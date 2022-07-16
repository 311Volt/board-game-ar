package butowska.anna.test2;

import android.net.Uri;

import androidx.lifecycle.ViewModel;

public class PhotoViewModel extends ViewModel {
    private Uri imageUri;

    public PhotoViewModel(Uri imageUri) {
        this.imageUri = imageUri;
    }

    public Uri getImageUri() {
        return imageUri;
    }
}
