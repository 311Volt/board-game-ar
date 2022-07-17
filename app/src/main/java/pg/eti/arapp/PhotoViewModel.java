package pg.eti.arapp;

import android.net.Uri;

import androidx.lifecycle.ViewModel;

public class PhotoViewModel extends ViewModel {
    private Uri imageUri;

    public PhotoViewModel() {
    }

    public Uri getImageUri() {
        return imageUri;
    }

    public void setImageUri(Uri imageUri) {
        this.imageUri = imageUri;
    }

}
