package pg.eti.arapp.ui.main_activity;

import android.net.Uri;

import androidx.lifecycle.ViewModel;

public class PhotoViewModel extends ViewModel {
    private Uri imageUri;
    private boolean isBoard;

    public PhotoViewModel() {
    }

    public Uri getImageUri() {
        return imageUri;
    }

    public void setImageUri(Uri imageUri) {
        this.imageUri = imageUri;
    }

    public boolean isBoard() {
        return isBoard;
    }

    public void setBoard(boolean board) {
        isBoard = board;
    }
}
