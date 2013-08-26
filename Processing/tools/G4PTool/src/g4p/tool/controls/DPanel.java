package g4p.tool.controls;

import g4p.tool.Messages;
import g4p.tool.gui.tabview.WindowView.MutableDBase;
import g4p_controls.GAlign;

import java.awt.Color;
import java.awt.Graphics2D;
import java.awt.geom.AffineTransform;
import java.io.IOException;
import java.io.ObjectInputStream;
import java.util.ArrayList;
import java.util.Enumeration;

@SuppressWarnings("serial")
public class DPanel extends DTextAlign {  // was DTextIcon now DText since no icon for this control

	final protected static int TAB_HEIGHT = 20;

	public Boolean 		_0300_collapsed = false;
	public String 		collapsed_label = "Collapsed?";
	public Boolean 		collapsed_edit = true;
	public Boolean 		collapsed_show = true;
	public String 		collapsed_updater = "collapsedChange";

	public Boolean 		_0301_collapsible = true;
	public String 		collapsible_label = "Collapsible?";
	public Boolean 		collapsible_edit = true;
	public Boolean 		collapsible_show = true;
	public String 		collapsible_updater = "collapsibleChange";

	public Boolean 		_0310_draggable = true;
	public String 		draggable_label = "Draggable?";
	public Boolean 		draggable_edit = true;
	public Boolean 		draggable_show = true;

	public String 		opaque_updater = "opaqueChange";

	public DPanel(){
		super();
		componentClass = "GPanel";
		set_name(NameGen.instance().getNext("panel"));
		set_event_name(NameGen.instance().getNext(get_name()+ "_Click"));
		allowsChildren = true;
		text_x_alignment_edit = text_x_alignment_show = false;
		text_y_alignment_edit = text_y_alignment_show = false;
		textHAlign = LEFT;
		textVAlign = TOP;
		_0130_text = "Tab bar text";
		text_label = "Panel tab text";
		text_tooltip = "text to appear in panel tab";
		_0826_width = 100;
		_0827_height = 60;
		_0600_opaque = true;
		opaque_edit = opaque_show = true;
	}

	public void opaqueChange(){
		if(!_0600_opaque){
			_0300_collapsed = false;
			_0301_collapsible = false;
			propertyModel.hasBeenChanged();
		}
	}

	public void collapsedChange(){
		// If we start collapsed and non collapsible the panel can never be expanded so fix
		if(_0300_collapsed && !_0301_collapsible){
			_0301_collapsible = true;
			propertyModel.hasBeenChanged();
		}
	}

	public void collapsibleChange(){
		// If not collaspible the panel must be expanded so fix
		if(!_0301_collapsible && _0300_collapsed){
			_0300_collapsed = false;
			propertyModel.hasBeenChanged();
		}
	}

	protected String get_creator(DBase parent, String window){
		String s;
		s = Messages.build(CTOR_GPANEL, _0010_name, window,
				$(_0820_x), $(_0821_y), $(_0826_width), $(_0827_height), _0130_text);
		if(_0300_collapsed)
			s += Messages.build(COLLAPSED, _0010_name, _0300_collapsed);
		if(!_0301_collapsible)
			s += Messages.build(COLLAPSIBLE, _0010_name, _0301_collapsible);
		if(!_0310_draggable)
			s += Messages.build(DRAGGABLE, _0010_name, _0310_draggable);
		s += super.get_creator(parent, window);		

		s += Messages.build(ADD_HANDLER, _0010_name, "this", _0020_eventHandler);
		return s;
	}

	public void make_creator(ArrayList<String> lines, DBase parent, String window){
		DBase comp;
		Enumeration<?> e;
		String ccode = get_creator(parent, window);
		if(ccode != null && !ccode.equals(""))
			lines.add(ccode);
		if(allowsChildren){
			e = children();
			while(e.hasMoreElements()){
				comp = (DBase)e.nextElement();
				comp.make_creator(lines, this, window);
			}
			e = children();
			while(e.hasMoreElements()){
				comp = (DBase)e.nextElement();
				if(!(comp instanceof DToggleGroup))
					lines.add(Messages.build(ADD_A_CHILD, _0010_name, comp._0010_name));
			}
		}				
	}

	public void draw(Graphics2D g, AffineTransform paf, DBase selected){
		AffineTransform af = new AffineTransform(paf);
		af.translate(_0820_x, _0821_y);
		g.setTransform(af);

		if(_0600_opaque){
			// Panel back
			if(!_0300_collapsed){
				g.setColor(jpalette[5]);
				g.fillRect(0, 0, _0826_width, _0827_height);
			}

			// Tab
			g.setColor(jpalette[4]);
			g.fillRect(0, 0, _0826_width, TAB_HEIGHT);

			// Text
//			g.setColor(jpalette[12]);
//			g.drawString(_0010_name, 2, 12);
		}
		else {
			g.setStroke(dashed);
			g.setColor(jpalette[2]);
			g.drawRect(0, 0, _0826_width, _0827_height);
		}
		
		super.draw(g, paf, selected); // draw text

		if(this == selected)
			drawSelector(g);

		if(!_0300_collapsed){
			Enumeration<?> e = children();
			while(e.hasMoreElements()){
				((DBase)e.nextElement()).draw(g, af, selected);
			}
		}
		g.setTransform(paf);
	}

	protected boolean isTextAlignDefaults(){
		return (textHAlign == LEFT && textVAlign == TOP);
	}

	public void drawSelector(Graphics2D g){
		g.setStroke(stdStroke);
		g.setColor(Color.red);
		g.drawRect(0, 0,_0826_width, _0827_height);

		drawHandle(g, _0826_width - HANDLE_SIZE, (_0827_height - HANDLE_SIZE)/2);
		drawHandle(g, (_0826_width - HANDLE_SIZE) / 2, _0827_height - HANDLE_SIZE);
		drawHandle(g, _0826_width - HANDLE_SIZE, _0827_height - HANDLE_SIZE);	
	}

	public void isOver(MutableDBase m, int x, int y) {
		if(selectable){
			x -= _0820_x;
			y -= _0821_y;
			//
			if(getSize() < m.area && isOverRectangle(x, y, 0, - TAB_HEIGHT, _0826_width, _0827_height + TAB_HEIGHT)){			
				m.selID = OVER_COMP;
				m.comp = this;
				m.area = getSize();
				if(isOverRectangle(x,y, _0826_width - HANDLE_SIZE, (_0827_height - HANDLE_SIZE)/2, HANDLE_SIZE, HANDLE_SIZE))
					m.selID = OVER_HORZ;
				else if(isOverRectangle(x,y, (_0826_width - HANDLE_SIZE) / 2, _0827_height - HANDLE_SIZE, HANDLE_SIZE, HANDLE_SIZE)) 
					m.selID = OVER_VERT;
				else if(isOverRectangle(x,y, _0826_width - HANDLE_SIZE, _0827_height - HANDLE_SIZE, HANDLE_SIZE, HANDLE_SIZE)) 
					m.selID = OVER_DIAG;
			}
		}
		if(this.allowsChildren){
			Enumeration<?> e = children();
			while(e.hasMoreElements()){
				((DBase)e.nextElement()).isOver(m, x, y);
			}
		}
	}

	/**
	 * Make allowance for the panel tab
	 */
	public boolean isOver(int x, int y){
		return (x >= _0820_x && x <= _0820_x + _0826_width 
				&& y >= _0821_y - TAB_HEIGHT && y <= _0821_y + _0827_height);
	}

	protected void read(){
		super.read();
	}

	private void readObject(ObjectInputStream in)
	throws IOException, ClassNotFoundException
	{
		in.defaultReadObject();
		read();
	}
}
