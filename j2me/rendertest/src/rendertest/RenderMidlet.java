/*
 * To change this template, choose Tools | Templates
 * and open the template in the editor.
 */
package rendertest;

import javax.microedition.lcdui.Display;
import javax.microedition.midlet.*;

/**
 * @author ilya
 */
public class RenderMidlet extends MIDlet {
    
    public void startApp() {
        Display.getDisplay(this).setCurrent(new SplashForm(this));
    }
    
    public void pauseApp() {
    }
    
    public void destroyApp(boolean unconditional) {
        notifyDestroyed();
    }
    
    public void showCanvas() {
        Display.getDisplay(this).setCurrent(getCanvas());
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
        showCanvas();
    }
    public void fontSelectorAccepted() {
        getCanvas().setFontSize(getFontSelector().getFontSize());
        showCanvas();
    }
    
    
    
    DebugPage debugPage = new DebugPage(this);
    DebugPage getDebugPage() {
        return debugPage;
    }
    
    void showDebugPage() {
        Display.getDisplay(this).setCurrent(getDebugPage());
    }
    
    void onDebugPageClosed() {
        showCanvas();
    }
    
    
    void onSplashFormClosed() {
        showCanvas();
    }
}
