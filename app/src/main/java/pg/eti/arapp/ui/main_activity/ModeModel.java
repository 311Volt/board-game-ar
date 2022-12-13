package pg.eti.arapp.ui.main_activity;

import androidx.lifecycle.ViewModel;

public class ModeModel extends ViewModel {
    private boolean isExperimental;

    public ModeModel() {
    }

    public void setExperimental(boolean experimental) {
        isExperimental = experimental;
    }

    public boolean isExperimental() {
        return isExperimental;
    }
}
