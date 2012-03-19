package meebible;

import javax.microedition.lcdui.*;

public class SplashForm extends Form implements CommandListener {

    MeeBibleMidlet midlet;
    
    Command cmdOK;
    
    SplashForm(MeeBibleMidlet midlet) {
        super("Render test");
        
        append(new StringItem("MIDlet started", ""));
        
        this.midlet = midlet;
        
        cmdOK = new Command("Start", Command.OK, 1);
        addCommand(cmdOK);
        setCommandListener(this);
    }
    
    public void commandAction(Command cmd, Displayable d) {
        if (cmd == cmdOK)
            midlet.onSplashFormClosed();
    }
    
}
