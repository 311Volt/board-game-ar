package pg.eti.arapp.catan;

import android.content.Context;
import android.content.res.AssetManager;
import android.graphics.Bitmap;
import android.graphics.BitmapFactory;
import android.graphics.ImageDecoder;
import android.util.Log;

import com.googlecode.tesseract.android.TessBaseAPI;

import org.opencv.android.Utils;
import org.opencv.core.Core;
import org.opencv.core.Mat;
import org.opencv.core.Point;
import org.opencv.core.Rect;
import org.opencv.imgcodecs.Imgcodecs;

import java.io.ByteArrayOutputStream;
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

    private void setupOCR(Context context, String language){

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

    // recognition part
    public Mat cutOutCardBottom(Mat card, float bottomAreaCoeff)
    {
        Rect bottomBoundingRect = new Rect(new Point(0, card.size().height * (1.0 - bottomAreaCoeff)), new Point(card.size().width - 1, card.size().height - 1));
        Mat cardBottom = card.submat(bottomBoundingRect);//card(bottomBoundingRect);
        return cardBottom;
    }

    public Mat cutOutCardHeading(Mat card, float headingAreaCoeff)
    {
        Rect headingBoundingRect = new Rect(new Point(0, 0), new Point(card.size().width - 1, card.size().height * headingAreaCoeff - 1));
        Mat cardBottom = card.submat(headingBoundingRect);// card(headingBoundingRect);
        return cardBottom;
    }

    private String prepareString(String text)
    {
        String s = text.replaceAll("[^a-zA-Z]", ""); //trimNonAlphabethical
        s = s.toUpperCase();
        return s;
    }

    private int assignCardTypeBasedOnText(String cardText)
    {
        /*String[] cardsCaptions = {"Koszty budowy", "Rycerz", "Katedra", "Ratusz", "Biblioteka", "Rynek", "Uniwersytet", "Postęp",
            "Najwyższa Władza Rycerska 2 Punkty Zwycięstwa", "Najdłuższa Droga Handlowa 2 Punkty Zwycięstwa" };*/
        String[] twoPointCardsContents = { "Najwyższa",  "Władza", "Rycerska", "Najdłuższa", "Droga Handlowa", "trzy karty rycerz", "pięć połączonych dróg", "Punkty Zwycięstwa" }; // "2 Punkty Zwyci�stwa"
        String[] onePointCardsContents = { /*"1",*/  "Punkt", "Zwycięstwa", "Katedra", "Ratusz", "Biblioteka", "Rynek", "Uniwersytet" };
        cardText = prepareString(cardText);

        String[] costsTableContents = { "Koszty budowy", "Rozwój", "pkt"};
        for (String templateText : costsTableContents)
        {
            //std::cout << "templateText: " << prepareString(templateText) << std::endl;

            if (cardText.contains(prepareString(templateText)))
                return 0;//scoringCardType::OTHER;
        }

        for (String templateText : twoPointCardsContents)
        {
            //std::cout << "templateText: " << prepareString(templateText) << std::endl;

            if (cardText.contains(prepareString(templateText)))
                return 2;//scoringCardType::TWO_POINTS;
        }
        for (String templateText : onePointCardsContents)
        {
            //std::cout << "templateText: " << prepareString(templateText) << std::endl;
            if (cardText.contains(prepareString(templateText)))
                return 1;//scoringCardType::ONE_POINT;
        }

        return 0;//scoringCardType::OTHER;
    }

    public int recognizeCard(Mat card, boolean isPlasticVer)
    {
        Mat cardCopy = new Mat(); //Mat.zeros(card.size(), card.type())
        card.copyTo(cardCopy);
        String outText = null;
        int cardType = 0;
        Mat cardPart;
        for (int a = 0; a < 2; a++)
        {
            if(!isPlasticVer)
                cardPart = cutOutCardHeading(cardCopy, 0.2f);
            else
                cardPart = cutOutCardBottom(cardCopy, 0.4f);

            Bitmap bmp = this.convertMatToBitmap(cardPart);
            this.tessBaseAPI.setImage(bmp);
            outText = this.tessBaseAPI.getUTF8Text();
            Log.d("Cards", "Card text"+outText);
            //outText = recognizeTextOnImage(cardPart, this.tessBaseAPI);
            cardType = assignCardTypeBasedOnText(outText);

            //std::cout << "OCR output:\n" << prepareString(std::string(outText)) << std::endl;
            //std::cout << "Recognition:\n" << cardTypeToString(cardType) << std::endl;
            //cv::imshow("Card part", cardPart);
            //cv::waitKey();

            if (cardType != 0)
                return cardType;
            Core.rotate(cardCopy, cardCopy, Core.ROTATE_180);
        }

        return cardType;
    }

    public Bitmap convertMatToBitmap(Mat mat)
    {
        Bitmap bmp = Bitmap.createBitmap(mat.width(), mat.height(), Bitmap.Config.ARGB_8888);
        Utils.matToBitmap(mat, bmp);
        return bmp;
    }

    public Mat convertBitmapToMat(Bitmap bmp)
    {
        Bitmap bmp32 = bmp.copy(Bitmap.Config.ARGB_8888, true);
        Mat mat = new Mat();
        Utils.bitmapToMat(bmp32, mat);
        return mat;
    }



}
