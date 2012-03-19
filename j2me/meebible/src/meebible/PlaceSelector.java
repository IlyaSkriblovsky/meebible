package meebible;

import javax.microedition.lcdui.*;

public class PlaceSelector extends Form implements CommandListener {

    static interface Listener {
        void selected(int bookNo, int chapterNo, int verseNo);
        void cancelled();
    }

    Book[] books;
    Listener listener;

    ChoiceGroup bookChoice = new ChoiceGroup("Book", Choice.POPUP);
    TextField chapterField = new TextField("Chapter", null, 3, TextField.NUMERIC);
    TextField verseField = new TextField("Verse", null, 3, TextField.NUMERIC);

    Command cmdBack = new Command("Back", Command.BACK, 1);
    Command cmdOk = new Command("Open", Command.OK, 1);

    PlaceSelector(Book[] books, int selectedBook, int chapterNo, int verseNo, Listener listener) {
        super("Select place");
        
        this.books = books;
        this.listener = listener;
        
        for (int i = 0; i < books.length; i++)
            bookChoice.append(books[i].name, null);
        bookChoice.setSelectedIndex(selectedBook, true);
        
        chapterField.setString(new Integer(chapterNo).toString());
        verseField.setString(new Integer(verseNo).toString());
        
        append(bookChoice);
        append(chapterField);
        append(verseField);
        
        setCommandListener(this);
        addCommand(cmdBack);
        addCommand(cmdOk);
    }

    public void commandAction(Command c, Displayable d) {
        if (c == cmdBack)
            listener.cancelled();
        else if (c == cmdOk) {
            int chapterNo = 1, verseNo = 1;
            try { chapterNo = Integer.parseInt(chapterField.getString()); }
            catch (NumberFormatException e) { }
            try { verseNo = Integer.parseInt(verseField.getString()); }
            catch (NumberFormatException e) { }

            listener.selected(bookChoice.getSelectedIndex(), chapterNo, verseNo);
        }
    }
    
}
