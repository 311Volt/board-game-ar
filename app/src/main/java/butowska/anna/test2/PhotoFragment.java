package butowska.anna.test2;

import android.net.Uri;
import android.os.Bundle;

import androidx.annotation.NonNull;
import androidx.fragment.app.Fragment;
import androidx.navigation.fragment.NavHostFragment;

import android.view.LayoutInflater;
import android.view.View;
import android.view.ViewGroup;

import butowska.anna.test2.databinding.FragmentLarpsBinding;
import butowska.anna.test2.databinding.FragmentPhotoBinding;

public class PhotoFragment extends Fragment {

    private FragmentPhotoBinding binding;
    private Uri selectedImageUri;

    public PhotoFragment() {}

    public PhotoFragment(Uri selectedImageUri) {
        PhotoFragment f = new PhotoFragment();
        this.selectedImageUri = selectedImageUri;
    }

    @Override
    public View onCreateView(LayoutInflater inflater, ViewGroup container,
                             Bundle savedInstanceState) {

        binding = FragmentPhotoBinding.inflate(inflater, container, false);
        return binding.getRoot();
    }

    public void onViewCreated(@NonNull View view, Bundle savedInstanceState) {
        super.onViewCreated(view, savedInstanceState);

        binding.imageView3.setImageURI(selectedImageUri);

        binding.buttonBack.setOnClickListener(new View.OnClickListener() {
            @Override
            public void onClick(View view) {
                NavHostFragment.findNavController(PhotoFragment.this)
                        .navigate(R.id.action_photoFragment_to_SecondFragment);
            }
        });
    }
}