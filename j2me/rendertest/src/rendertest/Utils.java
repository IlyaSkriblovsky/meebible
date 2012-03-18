package rendertest;

public class Utils {
    
    interface StringPartCallback {
        void onPart(String part);
    }

    static void forEachStringPart(String content, char separator, StringPartCallback callback) {
        int start = 0;
        
        int delim = content.indexOf(separator);
        while (delim != -1) {
            callback.onPart(content.substring(start, delim));
            start = delim+1;
            delim = content.indexOf(separator, start);
        }
        
        callback.onPart(content.substring(start));
    }
    
    static String[] split(String content, char separator) {
        int count = 1;
        int delim = content.indexOf(separator);
        while (delim != -1) {
            count++;
            delim = content.indexOf(separator, delim+1);
        }
        
        String[] result = new String[count];
        
        int start = 0;
        int i = 0;
        delim = content.indexOf(separator);
        while (delim != -1) {
            result[i++] = content.substring(start, delim);
            start = delim+1;
            delim = content.indexOf(separator, start);
        }
        
        result[i] = content.substring(start);
        
        return result;
    }
    
}
