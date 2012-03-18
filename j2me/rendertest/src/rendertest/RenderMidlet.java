package rendertest;

import javax.microedition.lcdui.Display;
import javax.microedition.lcdui.Displayable;
import javax.microedition.midlet.MIDlet;

public class RenderMidlet extends MIDlet {
    
    static RenderMidlet instance = null;
    
    public void startApp() {
        instance = this;
        
        // Display.getDisplay(this).setCurrent(new SplashForm(this));
        
        // getLangTransDialog().open();
        show(getCanvas());
    }
    
    public void pauseApp() {
    }
    
    public void destroyApp(boolean unconditional) {
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
    
    
    
    DebugPage debugPage = new DebugPage(this);
    DebugPage getDebugPage() {
        return debugPage;
    }
    
    void showDebugPage() {
        Display.getDisplay(this).setCurrent(getDebugPage());
    }
    
    void onDebugPageClosed() {
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
