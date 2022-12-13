package pg.eti.arapp.catan;

import android.content.Context;
import android.content.res.AssetManager;
import android.os.Environment;
import android.util.Log;

import com.googlecode.tesseract.android.TessBaseAPI;

import java.io.File;
import java.io.FileOutputStream;
import java.io.IOException;
import java.io.InputStream;
import java.io.OutputStream;
import java.util.ArrayList;

import pg.eti.arapp.detectortl.BufferBitmap;

public class CatanCardsDetector {

    private TessBaseAPI tessBaseAPI;

    public native ArrayList<BufferBitmap> getCardsNative(BufferBitmap bitmap);

    public void initTesseract(Context context, String language)
    {
        this.tessBaseAPI = new TessBaseAPI();

        this.setupOCR(context, language);

        File dataPath = new File(context.getFilesDir()+"/tesseract");
        if(dataPath.exists()) {
            if (!this.tessBaseAPI.init(dataPath.getAbsolutePath(), language)) {
                // Error initializing Tesseract (wrong data path or language)
                //this.tessBaseAPI.recycle();
                this.freeTesseract();
                Log.d("Tess", "Nooo ;-;");
            } else
                Log.d("Tess", "Yes :D");
        }

    }

    public void freeTesseract()
    {
        if(this.tessBaseAPI != null)
            this.tessBaseAPI.recycle();
    }

    public void setupOCR(Context context, String language){

        File folder = new File(context.getFilesDir() + "/tesseract/tessdata");
        if (!folder.exists()) {
            folder.mkdirs();
        }
        Log.d("Cards", "Folder "+ (folder.exists() ? "exists" : "doesn't exist"));
        File saving = new File(folder, language+".traineddata");
        try {
            saving.createNewFile();
        } catch (IOException e) {
            e.printStackTrace();
        }
        Log.d("Cards", "File "+ (saving.exists() ? "exists" : "doesn't exist"));
        InputStream stream = null;
        try {
            stream = context.getAssets().open(language+".traineddata", AssetManager.ACCESS_STREAMING);
        } catch (IOException e) {
            e.printStackTrace();
        }

        if (stream != null){
            copyInputStreamToFile(stream, saving);
        }
    }

    private void copyInputStreamToFile( InputStream in, File file ) {
        Log.d("Cards", "Stream exists");
        try {
            OutputStream out = new FileOutputStream(file);
            byte[] buf = new byte[1024];
            int len;
            while((len=in.read(buf))>0){
                out.write(buf,0,len);
            }
            out.close();
            in.close();
        } catch (Exception e) {
            e.printStackTrace();
        }
    }

}
