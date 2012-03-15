/*
 * To change this template, choose Tools | Templates
 * and open the template in the editor.
 */
package handtest;

import javax.microedition.midlet.*;
import javax.microedition.lcdui.*;

public class HandTestMidlet extends MIDlet {

    public void startApp() {
        Display.getDisplay(this).setCurrent(new MyCanvas());
    }
    
    public void pauseApp() {
    }
    
    public void destroyApp(boolean unconditional) {
    }
    
    
    
    private Form form;
    private Form getForm() {
        if (form == null) {
            form = new Form("Hello", new Item[] {
                getStringItem(),
                new TextField("Label", "text", 10, 0)
            });
            
            form.append("Static text");
            form.append("Another text");
        }
        return form;
    }
    
    
    private StringItem stringItem;
    private StringItem getStringItem() {
        if (stringItem == null) {
            stringItem = new StringItem("Title", "content", Item.BUTTON);
        }
        return stringItem;
    }
}
