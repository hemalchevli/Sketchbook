package g4p.tool.gui.tabview;

import g4p.tool.controls.DBase;

public interface ITabView {

	/**
	 * Add a newly created window component as a tab.
	 * 
	 * @param window
	 */
	public abstract void addWindow(DBase window);

	/**
	 * Remove a tab for a window that is no longer needed
	 * @param window
	 * @return
	 */
	public abstract boolean deleteWindow(DBase window);

	public void deleteAllWindows();
	
	/** 
	 * Change the selected tab to the window with this control
	 * @param comp
	 */
	public abstract void setSelectedComponent(DBase comp);

	/**
	 * Called when window name is changed in property view
	 */
	public void updateTabName();

	public void repaint();
	
	/**
	 * This method should be called by the WindowView when a 
	 * different component has been selected. 
	 * 
	 * @param comp the new component selected
	 */
	public abstract void componentHasBeenSelected(DBase comp);
	
	/**
	 * The size or position of the component has changed in
	 * the GUI tab using the mouse.
	 * @param comp
	 */
	public abstract void componentChangedInGUI(DBase comp);
	
	
	public abstract void scaleWindowToFit();
	public abstract void scaleWindow(int scale);
	
	public abstract void setShowGrid(boolean show);
	public abstract void setSnapToGrid(boolean snap);
	public abstract void setGridSize(int gsize);
	
}