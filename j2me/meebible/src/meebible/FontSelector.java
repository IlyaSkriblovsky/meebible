package meebible;

import javax.microedition.lcdui.*;

public class FontSelector extends List implements CommandListener {
    
    MeeBibleMidlet midlet;
    
    Command cmdCancel;
    Command cmdSelect;
    
    FontSelector(MeeBibleMidlet midlet) {
        super("Font size", List.IMPLICIT, new String[] { "Small", "Medium", "Large" }, null);

        this.midlet = midlet;
        
        setCommandListener(this);
        cmdCancel = new Command("Back", Command.BACK, 1);
        cmdSelect = new Command("Select", Command.ITEM, 1);
        setSelectCommand(cmdSelect);
        addCommand(cmdCancel);
    }
    
    public void commandAction(Command command, Displayable dislpayable) {
        if (command == cmdCancel) {
            midlet.fontSelectorCancelled();
        }
        else if (command == cmdSelect)
            midlet.fontSelectorAccepted();
    }
    
    
    public void setFontSize(int fontSize) {
        switch (fontSize) {
            case Font.SIZE_SMALL:  setSelectedIndex(0, true); break;
            case Font.SIZE_LARGE:  setSelectedIndex(2, true); break;
            default: setSelectedIndex(1, true); break;
        }
    }
    
    public int getFontSize() {
        switch (getSelectedIndex()) {
            case 0:  return Font.SIZE_SMALL;
            case 2:  return Font.SIZE_LARGE;
            default: return Font.SIZE_MEDIUM;
        }
    }
}
