package g4p.tool.controls;

import g4p.tool.Messages;
import g4p.tool.gui.propertygrid.EditorBase;
import g4p.tool.gui.propertygrid.EditorJComboBox;
import g4p.tool.gui.propertygrid.EditorJFileChooser;
import g4p.tool.gui.propertygrid.Validator;

import java.awt.Graphics2D;
import java.awt.geom.AffineTransform;
import java.awt.image.BufferedImage;

@SuppressWarnings("serial")
public class DTextIconAlign extends DTextAlign {

	transient protected BufferedImage icon = null;

	protected int iconX, iconY, iconWidth, iconHeight;
	protected int iconHAlign, iconVAlign;//, iconAlignModel;;
	public int iconNo = 0;
	
	public String 		_0150_icon_file = "";
	transient public 	EditorJFileChooser icon_file_editor = new EditorJFileChooser();
	public Boolean 		icon_file_edit = true;
	public Boolean 		icon_file_show = true;
	public String 		icon_file_label = "Icon file";
	public String 		icon_file_updater = "iconChanged";

	public int	 		_0152_nbr_tiles = 1;
	public Boolean 		nbr_tiles_edit = true;
	public Boolean 		nbr_tiles_show = false;
	public String 		nbr_tiles_label = "Nbr of tiles in icon";
	public String 		nbr_tiles_updater = "nbrTilesChanged";
	public Validator 	nbr_tiles_validator = Validator.getValidator(int.class, 1, 3);

	public String 		_0154_icon_x_alignment = "RIGHT";
	transient public 	EditorBase icon_x_alignment_editor = new EditorJComboBox(H_ALIGN_2);
	public Boolean 		icon_x_alignment_edit = true;
	public Boolean 		icon_x_alignment_show = false;
	public String 		icon_x_alignment_label = "Icon X align";
	public String 		icon_x_alignment_updater = "iconAlignChanged";

	public String 		_0155_icon_y_alignment = "MIDDLE";
	transient public 	EditorBase icon_y_alignment_editor = new EditorJComboBox(V_ALIGN);
	public Boolean 		icon_y_alignment_edit = true;
	public Boolean 		icon_y_alignment_show = false;
	public String 		icon_y_alignment_label = "Icon Y align";
	public String 		icon_y_alignment_updater = "iconAlignChanged";

	public DTextIconAlign(){
		super();
		iconHAlign = ListGen.instance().getIndexOf(H_ALIGN_2, _0154_icon_x_alignment);
		iconVAlign = ListGen.instance().getIndexOf(V_ALIGN, _0155_icon_y_alignment);
	}

	protected String get_creator(DBase parent, String window){
		String s = "";
		if(_0150_icon_file.length() > 0){
			s = Messages.build(SET_ICON, _0010_name, _0150_icon_file, _0152_nbr_tiles, _0154_icon_x_alignment, _0155_icon_y_alignment);
		}
		else if(icon != null && !isIconAlignDefaults()){
			s = Messages.build(SET_ICON_ALIGN, _0010_name, _0154_icon_x_alignment, _0155_icon_y_alignment);
		}
		s += super.get_creator(parent, window);
		return s;
	}

	// Override this method if needed
	public void textChanged(){
		super.textChanged();
	}
	
	public String get_text(){
		return _0130_text;
	}
	
	public void updatedInGUI(){
		sizeChanged();
	}

	/**
	 * If the width or height is changed then we need to update the text etc.
	 */
	public void sizeChanged(){
		textWidth = _0826_width;
		iconAlignChanged();
	}

	public void nbrTilesChanged(){
		iconWidth = icon.getWidth() / _0152_nbr_tiles;		
		iconAlignChanged();
	}

	public void iconAlignChanged(){
		iconHAlign = ListGen.instance().getIndexOf(H_ALIGN_2, _0154_icon_x_alignment);
		iconVAlign = ListGen.instance().getIndexOf(V_ALIGN, _0155_icon_y_alignment);
		if(iconHAlign == LEFT){
			iconX = 0;
			setHorzTextBoxValues(iconWidth, _0826_width - iconWidth);
		}
		else {
			iconX = _0826_width - iconWidth;
			setHorzTextBoxValues(0, _0826_width - iconWidth);
		}
		switch(iconVAlign){
		case TOP:
			iconY = 0;
			break;
		case MIDDLE:
			iconY = (_0827_height - iconHeight)/2;
			break;
		case BOTTOM:
			iconY = _0827_height - iconHeight;
			break;
		}
//		propertyModel.hasBeenChanged();
	}

	protected boolean isIconAlignDefaults(){
		return _0154_icon_x_alignment.equals("RIGHT") && _0155_icon_y_alignment.equals("MIDDLE");
	}

	public void iconChanged(){
		icon = this.getImageFromDataFolder(_0150_icon_file);
		if(icon != null){
			iconWidth = icon.getWidth() / _0152_nbr_tiles;
			iconHeight = icon.getHeight();
			if(iconWidth > _0826_width)
				_0826_width = _0826_width + 30;
			if(iconHeight > _0827_height)
				_0827_height = iconHeight + 4;
			nbr_tiles_show = true;
			icon_x_alignment_show = true;
			icon_y_alignment_show = true;
			iconAlignChanged();
			propertyModel.createProperties(this);
			propertyModel.hasBeenChanged();
		}
	}

	public void draw(Graphics2D g, AffineTransform paf, DBase selected){
		super.draw(g, paf, selected); // draw text
		if(icon != null)
			g.drawImage(icon, iconX, iconY, iconX + iconWidth, iconY + iconHeight, 
					iconNo * iconWidth, 0, iconNo * iconWidth + iconWidth, iconHeight, null);
	}

	protected void read(){
		super.read();

		icon_file_editor = new EditorJFileChooser();
		icon_x_alignment_editor = new EditorJComboBox(H_ALIGN_3);
		icon_y_alignment_editor = new EditorJComboBox(V_ALIGN);
		if(_0150_icon_file.length() > 0)
			icon = getImageFromDataFolder(_0150_icon_file);
	}
	
}
