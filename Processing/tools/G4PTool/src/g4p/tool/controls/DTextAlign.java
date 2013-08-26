package g4p.tool.controls;

import g4p.tool.Messages;
import g4p.tool.gui.propertygrid.EditorBase;
import g4p.tool.gui.propertygrid.EditorJComboBox;
import g4p_controls.G4P;
import g4p_controls.StyledString;

@SuppressWarnings("serial")
public class DTextAlign extends DTextBase {

	public Boolean 		_0136_bold  = false;
	public Boolean 		bold_edit = true;
	public Boolean 		bold_show = true;
	public String 		bold_label = "Bold";
	public String 		bold_updater = "updateStyle";

	public Boolean 		_0137_italic  = false;
	public Boolean 		italic_edit = true;
	public Boolean 		italic_show = true;
	public String 		italic_label = "Italic";
	public String 		italic_updater = "updateStyle";

	public String 		width_updater = "sizeChanged";
	public String 		height_updater = "sizeChanged";

	public String 		_0140_text_x_alignment = "CENTER";
	transient public 	EditorBase text_x_alignment_editor = new EditorJComboBox(H_ALIGN_3);
	public Boolean 		text_x_alignment_edit = true;
	public Boolean 		text_x_alignment_show = true;
	public String 		text_x_alignment_label = "Text X align";
	public String 		text_x_alignment_updater = "textAlignChanged";

	public String 		_0141_text_y_alignment = "MIDDLE";
	transient public 	EditorBase text_y_alignment_editor = new EditorJComboBox(V_ALIGN);
	public Boolean 		text_y_alignment_edit = true;
	public Boolean 		text_y_alignment_show = true;
	public String 		text_y_alignment_label = "Text Y align";
	public String 		text_y_alignment_updater = "textAlignChanged";
	
	
	public DTextAlign(){
		super();
		textHAlign = ListGen.instance().getIndexOf(H_ALIGN_3, _0140_text_x_alignment);
		textVAlign = ListGen.instance().getIndexOf(V_ALIGN, _0141_text_y_alignment);
	}

	public void textChanged(){
//		textWidthChanged = true;
		stext = new StyledString(_0130_text, textWidth);
		if(text_x_alignment_edit || text_y_alignment_edit){
			int currLength = _0130_text.length();
			if(currLength != lastLength){
				// Do we have to change the show alignment property.
				// If yes then the property model needs updating
				if(currLength == 0 || lastLength == 0){
					boolean hasText = (_0130_text.length() != 0);
					text_x_alignment_show = hasText;
					text_y_alignment_show = hasText;
					italic_show = hasText;
					bold_show = hasText;
					if(!hasText){
						_0136_bold = false;
						_0137_italic = false;
					}
					propertyModel.createProperties(this);
				}
				lastLength = currLength; // remember the current length
			}
		}
		propertyModel.hasBeenChanged();
	}

	public void updateStyle(){
		stext.clearAllAttributes();
		if(_0136_bold)
			stext.addAttribute(G4P.WEIGHT, G4P.WEIGHT_BOLD);
		if(_0137_italic)
			stext.addAttribute(G4P.POSTURE, G4P.POSTURE_OBLIQUE);
	}

	protected String get_creator(DBase parent, String window){
		String s = "";
		if(_0136_bold)
			s += Messages.build(SET_TEXT_BOLD, _0010_name);
		if(_0137_italic)
			s += Messages.build(SET_TEXT_ITALIC, _0010_name);
		if(_0130_text.length() > 0 && !isTextAlignDefaults())
				s += Messages.build(SET_TEXT_ALIGN, _0010_name, _0140_text_x_alignment, _0141_text_y_alignment);
		s += super.get_creator(parent, window);		
		return s;
	}

	protected boolean isTextAlignDefaults(){
		return (textHAlign == CENTER && textVAlign == MIDDLE);
	}


	public void textAlignChanged(){
		textHAlign = ListGen.instance().getIndexOf(H_ALIGN_3, _0140_text_x_alignment);
		textVAlign = ListGen.instance().getIndexOf(V_ALIGN, _0141_text_y_alignment);
	}
	
	
	protected void read(){
		super.read();
		text_x_alignment_editor = new EditorJComboBox(H_ALIGN_3);
		text_y_alignment_editor = new EditorJComboBox(V_ALIGN);
	}

}
