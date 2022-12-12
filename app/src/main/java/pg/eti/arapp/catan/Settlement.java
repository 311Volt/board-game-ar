package pg.eti.arapp.catan;

import java.util.Arrays;
import java.util.stream.Collectors;

public class Settlement {
    public PlayerColor playerColor;
    public boolean isCity;

    public Settlement(PlayerColor playerColor, boolean isCity) {
        this.playerColor = playerColor;
        this.isCity = isCity;
    }

    public static Settlement parse(String str) {
        String[] split = Arrays.stream(str.split(" ")).filter(s -> !s.isEmpty()).toArray(String[]::new);
        return new Settlement(PlayerColor.parse(split[0]), Boolean.parseBoolean(split[1]));
    }
}
