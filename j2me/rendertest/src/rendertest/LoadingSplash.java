package rendertest;

import javax.microedition.lcdui.Displayable;
import javax.microedition.lcdui.Form;
import javax.microedition.lcdui.Gauge;

public class LoadingSplash extends Form {
    
    LoadingSplash(String text) {
        super(text);
        append(new Gauge("Please wait...", false, Gauge.INDEFINITE, Gauge.CONTINUOUS_RUNNING));
        
        RenderMidlet.instance.show(this);
    }
    
    void close(Displayable d) {
        RenderMidlet.instance.show(d);
    }
    
}
