package pg.eti.arapp.ui.main_activity;

import android.graphics.Bitmap;
import android.net.Uri;
import android.os.Bundle;
import android.provider.MediaStore;
import android.view.LayoutInflater;
import android.view.View;
import android.view.ViewGroup;
import android.widget.TextView;

import androidx.annotation.NonNull;
import androidx.fragment.app.Fragment;
import androidx.lifecycle.ViewModelProvider;
import androidx.navigation.fragment.NavHostFragment;

import java.io.IOException;
import java.util.ArrayList;
import java.util.HashMap;
import java.util.List;
import java.util.Optional;

import pg.eti.arapp.R;
import pg.eti.arapp.catan.BoardInfo;
import pg.eti.arapp.catan.CatanBoardDetector;
import pg.eti.arapp.catan.Player;
import pg.eti.arapp.catan.PlayerColor;
import pg.eti.arapp.catan.Settlement;
import pg.eti.arapp.catan.coord.VertexCoord;
import pg.eti.arapp.databinding.FragmentPhotoBinding;
import pg.eti.arapp.detectortl.BufferBitmap;

public class PhotoFragment extends Fragment {

    private FragmentPhotoBinding binding;

    private PhotoViewModel photoViewModel;

    @Override
    public View onCreateView(LayoutInflater inflater, ViewGroup container,
                             Bundle savedInstanceState) {

        binding = FragmentPhotoBinding.inflate(inflater, container, false);
        return binding.getRoot();
    }

    public void onViewCreated(@NonNull View view, Bundle savedInstanceState) {
        super.onViewCreated(view, savedInstanceState);

        Uri selectedImageUri;
        photoViewModel = new ViewModelProvider(requireActivity()).get(PhotoViewModel.class);
        selectedImageUri = photoViewModel.getImageUri();

        binding.imageView3.setImageURI(selectedImageUri);

        binding.buttonBack.setOnClickListener(new View.OnClickListener() {
            @Override
            public void onClick(View view) {
                NavHostFragment.findNavController(PhotoFragment.this)
                        .navigate(R.id.action_photoFragment_to_SecondFragment);
            }
        });

        binding.buttonAnalyze.setOnClickListener(new View.OnClickListener() {
            @Override
            public void onClick(View view) {
                try {
                    if(photoViewModel.isBoard())
                        AnalyseBoard(selectedImageUri);
                    else
                        AnalyseCards(selectedImageUri);
                } catch (IOException e) {
                    e.printStackTrace();
                }
            }
        });
    }

    public void AnalyseBoard(Uri imageUri) throws IOException {
        List<Player> players = new ArrayList<>();
        Bitmap bitmap = MediaStore.Images.Media.getBitmap(this.getActivity().getContentResolver(), imageUri);
        CatanBoardDetector detector = new CatanBoardDetector();

        if (bitmap != null) {
            BoardInfo boardInfo = detector.analyze(new BufferBitmap(bitmap));
            TextView view = getActivity().findViewById(R.id.score_view_board);
            for (HashMap.Entry<VertexCoord, Settlement> settlementEntry: boardInfo.settlements.entrySet()) {
                Optional<Player> optionalPlayer = players.stream().filter(p -> p.getColor() == settlementEntry.getValue().playerColor).findAny();
                Player player;
                if(!optionalPlayer.isPresent()){
                    player = new Player(settlementEntry.getValue().playerColor);
                    players.add(player);
                } else
                    player = optionalPlayer.get();
                player.AddPoints((short) (settlementEntry.getValue().isCity ? 2 : 1));
            }
            view.setText(String.format("%s", ScoreText(players)));
        }
    }

    public void AnalyseCards(Uri imageUri) throws IOException {
        Player player = new Player(PlayerColor.BLUE);
        Bitmap bitmap = MediaStore.Images.Media.getBitmap(this.getActivity().getContentResolver(), imageUri);

        if (bitmap != null) {
            TextView view = getActivity().findViewById(R.id.score_view_board);
            view.setTextSize(20);
            view.setText(player.AnalysedCards());
        }
    }

    private String ScoreText(List<Player> players){
        String result = "";
        for(Player p : players){
            result += p.toString() + "\n";
        }
        return result;
    }

}