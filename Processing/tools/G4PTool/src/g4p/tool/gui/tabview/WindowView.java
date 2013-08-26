/*
 * To change this template, choose Tools | Templates
 * and open the template in the editor.
 */

package g4p.tool.gui.tabview;

import g4p.tool.Messages;
import g4p.tool.TGuiConstants;
import g4p.tool.controls.DBase;
import g4p.tool.controls.DWindow;

import java.awt.Color;
import java.awt.Cursor;
import java.awt.Font;
import java.awt.Graphics;
import java.awt.Graphics2D;
import java.awt.event.MouseEvent;
import java.awt.event.MouseListener;
import java.awt.event.MouseMotionListener;
import java.awt.geom.AffineTransform;

import javax.swing.JPanel;

/**
 *
 * @author Peter
 */
@SuppressWarnings("serial")
public class WindowView extends JPanel 
implements  MouseListener, MouseMotionListener, TGuiConstants {

	private ITabView tabCtrl;

	public static int gridSize = 4;
	public static boolean showGrid;
	public static boolean snapToGrid;

	private static Color gridCol = new Color(32,64,32);

	private DBase window = null;
	private DBase selected;

	private MutableDBase selInfo = new MutableDBase();

	private int startX, startY;
	private int deltaX, deltaY;


	public WindowView(ITabView pane, DBase window){
		this.window = window;
		this.tabCtrl = pane;
		this.setBackground(new Color(255,255,255));
		setFont(displayFont);
		this.setCursor(new java.awt.Cursor(java.awt.Cursor.CROSSHAIR_CURSOR));
		addMouseListener(this);
		addMouseMotionListener(this);
		this.setFont(new Font("Dialog", Font.PLAIN, 12));

	}

	public DBase getWindowComponent(){
		return window;
	}

	public void paintComponent(Graphics g){
		super.paintComponent(g);
		Graphics2D g2 = (Graphics2D)g;
		float scale = ((DWindow)window)._0025_Display_scale / 100.0f;
		AffineTransform orgAF = g2.getTransform();
		AffineTransform af = new AffineTransform(orgAF);
		g2.setStroke(stdStroke);
		af.scale(scale, scale);
		if(showGrid)
			drawGrid(g2, gridSize, scale);
		window.draw(g2, af, selected);
		g2.setTransform(orgAF);
	}

	private void drawGrid(Graphics2D g, int gs, float scale){
		int w = getWidth();
		int h = getHeight();
		g.setColor(gridCol);
		int x = 0, y = 0;
		int nx = 0, ny = 0;
		while(y < h){
			x = 0;
			nx = 0;
			while(x < w){
				g.fillOval(x, y, 1, 1);
				nx++;
				x = Math.round(nx * gs * scale);
			}
			ny++;
			y = Math.round(ny * gs * scale);
		}					
	}

	public void isOver(MutableDBase m, int x, int y){
		float scale = ((DWindow)window)._0025_Display_scale / 100.0f;
		int sx = Math.round(x / scale);
		int sy = Math.round(y / scale);
		selInfo.reset();
		window.isOver(m, sx, sy);
		if(m.comp != null){
			m.orgX = m.comp._0820_x;
			m.orgY = m.comp._0821_y;
			m.orgW = m.comp._0826_width;
			m.orgH = m.comp._0827_height;
		}
	}

	public void setSelected(DBase comp){
		selected = comp;
	}


	/**
	 * @param scale the scale to set
	 */
	public void scaleWindowToFit(int w, int h) {
		int scale = Math.round(92.0f * Math.min(((float) w)/window._0826_width,
				((float) h)/window._0827_height));
		((DWindow)window)._0025_Display_scale = scale;
		repaint();
		tabCtrl.componentChangedInGUI(window);
	}

	void scaleWindowToFit(int scale) {
		((DWindow)window)._0025_Display_scale = scale;
		repaint();
		tabCtrl.componentChangedInGUI(window);
	}

	@Override
	public void mouseClicked(MouseEvent e) {
		if(selInfo.comp != selected){
			selected = selInfo.comp;
			tabCtrl.componentHasBeenSelected(selected);
		}	
	}

	@Override
	public void mousePressed(MouseEvent e) {
		startX = e.getX();
		startY = e.getY();
		isOver(selInfo, startX, startY);
		if(selInfo.comp != null && selInfo.comp.isSelectable()){
			selected = selInfo.comp;
			tabCtrl.componentHasBeenSelected(selected);
		}
	}

	@Override
	public void mouseReleased(MouseEvent e) {
		setCursor(Cursor.getPredefinedCursor(Cursor.CROSSHAIR_CURSOR));
	}

	@Override
	public void mouseEntered(MouseEvent e) {
	}

	@Override
	public void mouseExited(MouseEvent e) {
	}

	@Override
	public void mouseDragged(MouseEvent e) {
		if(selInfo.comp != null && selInfo.comp.isSelectable()){
			float scale = ((DWindow)window)._0025_Display_scale / 100.0f;
			deltaX = Math.round((e.getX() - startX) / scale);
			deltaY = Math.round((e.getY() - startY) / scale);
			if(selInfo.comp.isResizeable()){
				switch(selInfo.selID){
				case OVER_HORZ:
					setCursor(Cursor.getPredefinedCursor(Cursor.E_RESIZE_CURSOR));
					selInfo.comp._0826_width = snapValue(selInfo.orgW + deltaX);
					break;
				case OVER_VERT:
					setCursor(Cursor.getPredefinedCursor(Cursor.S_RESIZE_CURSOR));
					selInfo.comp._0827_height = snapValue(selInfo.orgH + deltaY);
					break;
				case OVER_DIAG:
					setCursor(Cursor.getPredefinedCursor(Cursor.NW_RESIZE_CURSOR));
					selInfo.comp._0826_width = snapValue(selInfo.orgW + deltaX);
					selInfo.comp._0827_height = snapValue(selInfo.orgH + deltaY);
					break;
				}
			}
			if(selInfo.comp.isMoveable() && selInfo.selID == OVER_COMP){
				setCursor(Cursor.getPredefinedCursor(Cursor.MOVE_CURSOR));
				selInfo.comp._0820_x = snapValue(selInfo.orgX + deltaX);
				selInfo.comp._0821_y = snapValue(selInfo.orgY + deltaY);
			}
			tabCtrl.componentChangedInGUI(selInfo.comp);
			repaint();
		}
	}

	private int snapValue(int nbr){
		return (snapToGrid) ? gridSize * Math.round(((float)nbr)/((float)gridSize)) : nbr;
	}

	@Override
	public void mouseMoved(MouseEvent e) {
	}

	public static class MutableDBase{
		public DBase comp = null;
		public int area = Integer.MAX_VALUE;
		public int selID = OVER_NONE;
		public int orgX,orgY,orgW,orgH;

		public MutableDBase(){	}

		public void reset(){
			comp = null;
			area = Integer.MAX_VALUE;
			selID = OVER_NONE;
		}

		public void reset(DBase comp){
			this.comp = comp;
			area = comp.getSize();
			selID = OVER_NONE;
		}

		public String toString(){
			return Messages.build("{0}   over  {1}", comp, selID);
		}
	}

}
