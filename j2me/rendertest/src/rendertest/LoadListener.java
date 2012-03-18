package rendertest;

public abstract class LoadListener {
    
    public abstract void finished(String content);
    public void progress(long done, long total) { }
    public abstract void error();
    
}
