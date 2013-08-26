package g4p.tool.gui.tabview;

import g4p.tool.controls.DBase;
import g4p.tool.controls.DWindow;
import g4p.tool.gui.ToolIcon;
import g4p.tool.gui.propertygrid.IPropView;
import g4p.tool.gui.treeview.ISketchView;

import java.awt.Graphics;
import java.util.HashMap;

import javax.swing.JTabbedPane;
import javax.swing.event.ChangeEvent;
import javax.swing.event.ChangeListener;

/**
 * 
 * @author Peter Lager
 *
 */
@SuppressWarnings("serial")
public class CtrlTabView extends JTabbedPane implements ITabView, ChangeListener {

	private ISketchView tree;
	private IPropView props;


	HashMap<DBase, WindowView> tabMap; 

	public CtrlTabView(){
		tabMap =  new HashMap<DBase, WindowView>();
		this.addChangeListener(this);
	}

	public void setViewLinks(ISketchView tree, IPropView props){
		this.tree = tree;
		this.props = props;
	}

	public void paintComponent(Graphics g){
		super.paintComponent(g);
		getSelectedComponent().repaint();
	}
	
	/* (non-Javadoc)
	 * @see g4p.tool.gui.IWindowView#addWindow(g4p.tool.components.DBase)
	 */
	@Override
	public void addWindow(DBase winComp){
		WindowView winView = new WindowView(this, winComp);
		tabMap.put(winComp, winView);
		addTab(winComp.get_name(), ToolIcon.getIcon(DWindow.class), winView);
	}

	/* (non-Javadoc)
	 * @see g4p.tool.gui.IWindowView#deleteWindow(g4p.tool.components.DBase)
	 */
	@Override
	public boolean deleteWindow(DBase window){
		WindowView winView = tabMap.get(window);
		if(winView != null){
			remove(winView); // remove tab
			tabMap.remove(window);
			return true;
		}
		return false;		
	}

	private void setSelectedTab(DBase window){
		WindowView winView = tabMap.get(window);
		if(winView != null)
			setSelectedComponent(winView);
	}

	/**
	 * Set selected component.
	 * If necessary change tab and then update component
	 * @param comp
	 */
	public void setSelectedComponent(DBase comp){
		// Clear current selection
		WindowView winView = (WindowView) getSelectedComponent();
		if(winView != null)
			winView.setSelected(null);
		winView = null;
		// Now check for component passed
		if(comp instanceof DWindow){
			winView = tabMap.get(comp); 
			setSelectedTab(comp);
		}
		else {
			DBase window = tree.getWindowFor(comp);
			if(window != null ){													// && window instanceof DWindow ???????
				setSelectedTab(window);
				// It it was something other than a window update the component
				// it is selecting
				if(window != comp){
					winView = tabMap.get(window);
					//					if(winView != null)
					//						winView.UpdateComponent(comp);
					tree.setSelectedComponent(comp);
				}
			}
		}
		if(winView != null)
			winView.setSelected(comp);
		repaint();
	}

	/**
	 * Call this if the name of the window has changed
	 */
	public void updateTabName(){
		for(int i = 0; i < this.getTabCount(); i++){
			String winname = ( (WindowView) getComponentAt(i)).getWindowComponent().get_name();
			setTitleAt(i, winname);
		}
	}

	/**
	 * Change of state caused by clicking on a tab
	 */
	@Override
	public void stateChanged(ChangeEvent e) {
		CtrlTabView sourceTabbedPane = (CtrlTabView) changeEvent.getSource();
		WindowView winView = (WindowView) sourceTabbedPane.getSelectedComponent();
		if(winView != null){
			DBase comp = winView.getWindowComponent();
			props.showProprtiesFor(comp);
			tree.setSelectedComponent(comp);
		}
	}

	/**
	 * Called from WindowView when a component is selected using the mouse
	 * From ITabView
	 */
	@Override
	public void componentHasBeenSelected(DBase comp) {
		if(comp != null)
			tree.setSelectedComponent(comp);		
	}

	@Override
	public void componentChangedInGUI(DBase comp) {
		comp.updatedInGUI();
		props.modelHasBeenChanged();
	}

	@Override
	public void scaleWindowToFit() {
		WindowView winView = (WindowView) this.getSelectedComponent();
		winView.scaleWindowToFit(getWidth(), getHeight());
	}

	@Override
	public void scaleWindow(int scale) {
		WindowView winView = (WindowView) this.getSelectedComponent();
		winView.scaleWindowToFit(scale);
	}


	@Override
	public void setGridSize(int gsize) {
		WindowView.gridSize = gsize;
		repaint();
	}

	@Override
	public void setShowGrid(boolean show) {
		WindowView.showGrid = show;		
		repaint();
	}

	@Override
	public void setSnapToGrid(boolean snap) {
		WindowView.snapToGrid = snap;		
		repaint();
	}

	@Override
	public void deleteAllWindows() {
		this.removeAll();

	}



}
