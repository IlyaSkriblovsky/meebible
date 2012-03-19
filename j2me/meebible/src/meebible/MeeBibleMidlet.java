package meebible;

import javax.microedition.lcdui.Display;
import javax.microedition.lcdui.Displayable;
import javax.microedition.midlet.MIDlet;
import javax.microedition.rms.RecordStore;
import javax.microedition.rms.RecordStoreException;
import javax.microedition.rms.RecordStoreFullException;
import javax.microedition.rms.RecordStoreNotFoundException;

public class MeeBibleMidlet extends MIDlet {
    
    static MeeBibleMidlet instance = null;
    
    public void startApp() {
        instance = this;
        
        // Display.getDisplay(this).setCurrent(new SplashForm(this));
        
        // getLangTransDialog().open();
        show(getCanvas());
    }
    
    public void pauseApp() {
    }
    
    public void destroyApp(boolean unconditional) {
        getCanvas().saveSettings();
        notifyDestroyed();
    }
    
    public void show(Displayable d) {
        Display.getDisplay(this).setCurrent(d);
    }
    
    public void showFontSelector() {
        FontSelector dialog = getFontSelector();
        dialog.setFontSize(getCanvas().getFontSize());
        Display.getDisplay(this).setCurrent(dialog);
    }
    
    RenderCanvas canvas;
    RenderCanvas getCanvas() {
        if (canvas == null)
            canvas = new RenderCanvas(this);
        return canvas;
    }
    
    FontSelector fontSelector;
    FontSelector getFontSelector() {
        if (fontSelector == null)
            fontSelector = new FontSelector(this);
        return fontSelector;
    }
    
    public void fontSelectorCancelled() {
        show(getCanvas());
    }
    public void fontSelectorAccepted() {
        getCanvas().setFontSize(getFontSelector().getFontSize());
        show(getCanvas());
    }
    
    
    
    void onSplashFormClosed() {
        show(getCanvas());
    }
    
    
    LangTransDialog langTransDialog;
    LangTransDialog getLangTransDialog() {
        if (langTransDialog == null)
            langTransDialog = new LangTransDialog();
        return langTransDialog;
    }
}
