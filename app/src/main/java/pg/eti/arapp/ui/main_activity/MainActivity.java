package pg.eti.arapp.ui.main_activity;

import android.content.Intent;
import android.os.Bundle;
import android.util.Log;
import android.view.Menu;
import android.view.MenuItem;

import androidx.appcompat.app.AppCompatActivity;
import androidx.lifecycle.ViewModelProvider;
import androidx.navigation.NavController;
import androidx.navigation.Navigation;

import org.opencv.android.OpenCVLoader;

import pg.eti.arapp.R;
import pg.eti.arapp.databinding.ActivityMainBinding;
import pg.eti.arapp.ui.ARActivity;


public class MainActivity extends AppCompatActivity {

    private ActivityMainBinding binding;

    private PhotoViewModel photoViewModel;

    private ModeModel modeModel;

    @Override
    protected void onCreate(Bundle savedInstanceState) {
        super.onCreate(savedInstanceState);
        binding = ActivityMainBinding.inflate(getLayoutInflater());
        setContentView(binding.getRoot());
        photoViewModel = new ViewModelProvider(this).get(PhotoViewModel.class);
        modeModel = new ViewModelProvider(this).get(ModeModel.class);
        NavController navController = Navigation.findNavController(this, R.id.nav_host_fragment_content_main);



        if(OpenCVLoader.initDebug()) {
            Log.d("OPENCV", "OpenCV initialized successfully");
        } else {
            Log.e("OPENCV", "Could not initialize OpenCV");
        }
    }

    @Override
    public boolean onCreateOptionsMenu(Menu menu) {
        getMenuInflater().inflate(R.menu.menu_main, menu);
        return true;
    }

    @Override
    public boolean onOptionsItemSelected(MenuItem item) {
        int id = item.getItemId();
        if (id == R.id.action_settings) {
            return true;
        }
        return super.onOptionsItemSelected(item);
    }

}