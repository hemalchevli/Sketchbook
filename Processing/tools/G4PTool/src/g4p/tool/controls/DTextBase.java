package g4p.tool.controls;

import g4p.tool.Messages;
import g4p.tool.gui.propertygrid.Validator;
import g4p_controls.StyledString;
import g4p_controls.StyledString.TextLayoutInfo;

import java.awt.Graphics2D;
import java.awt.font.TextLayout;
import java.awt.geom.AffineTransform;
import java.util.LinkedList;


@SuppressWarnings("serial")
public class DTextBase extends DBaseVisual {

	protected int textWidth, textX;
	protected int textHAlign, textVAlign;
	
	protected int lastLength;
	protected boolean textWidthChanged = true;
	transient public StyledString stext = null;
	
	public String 		_0130_text = "";
	public String 		text_label = "Text";
	public String 		text_tooltip = "component label text";
	public Boolean 		text_edit = true;
	public Boolean 		text_show = true;
	public Validator 	text_validator = Validator.getDefaultValidator(String.class);
	public String		text_updater = "textChanged";

	public DTextBase(){
		super();
		selectable = true;
		resizeable = true;
		moveable = true;
		allowsChildren = false;
		_0826_width = 80;
		_0827_height = 22;
		textX = 0;
		textWidth = _0826_width;
		lastLength = _0130_text.length();
		eventHandler_edit = eventHandler_show = true;
	}

	protected String get_creator(DBase parent, String window){
		String s = "";
		if(_0130_text.length() > 0)
			s = Messages.build(SET_TEXT, _0010_name, _0130_text);
		s += super.get_creator(parent, window);		
		return s;
	}

	// Override this method if needed
	public void textChanged(){
		stext = new StyledString(_0130_text, textWidth);
		propertyModel.hasBeenChanged();
	}

	/**
	 * This will be called if the text box has to me moved in a child class.
	 * e.g. if we have an icon
	 * @param x_offset
	 * @param text_width
	 */
	protected void setHorzTextBoxValues(int x_offset, int text_width){
		textX = x_offset;
		if(textWidth != text_width){
			textWidth = text_width;
		}
	}
	
	/**
	 * If the width or height is changed then we need to update the text etc.
	 */
	public void sizeChanged(){
		textWidth = _0826_width;
		textWidthChanged = true;
	}
	
	public String get_text(){
		return _0130_text;
	}

	public void draw(Graphics2D g, AffineTransform paf, DBase selected){
		if(textWidthChanged){
			if(stext == null)
				stext = new StyledString(_0130_text, textWidth);
			else
				stext.setWrapWidth(textWidth);
			textWidthChanged = false;
		}
		
		LinkedList<TextLayoutInfo> lines = stext.getLines(g);

		float deltaY = stext.getMaxLineHeight(), currY = 0, startX;

		for(TextLayoutInfo lineInfo : lines){
			TextLayout layout = lineInfo.layout;
			switch(textHAlign){
			case CENTER:
				startX = (textWidth - layout.getVisibleAdvance())/2;
				break;
			case RIGHT:
				startX = (textWidth - layout.getVisibleAdvance());
				break;
			case LEFT:
			default:
				startX = 0;		
			}
			switch(textVAlign){
			case TOP:
				currY = deltaY;
				break;
			case MIDDLE:
				currY = (_0827_height - stext.getTextAreaHeight())/2 + layout.getAscent();
				break;
			case BOTTOM:
				currY = _0827_height - stext.getTextAreaHeight() + deltaY - layout.getDescent();
			}

			// display text
			g.setColor(jpalette[2]);
			layout.draw(g, textX + startX, currY);
			currY += deltaY;
		}
	}
	
	protected void read(){
		super.read();
		if(stext == null){
			stext = new StyledString(_0130_text);
		}
	}


}
