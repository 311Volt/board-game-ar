package pg.eti.arapp.catan;

public class Settlement {
    public PlayerColor playerColor;
    public boolean isCity;

    public Settlement(PlayerColor playerColor, boolean isCity) {
        this.playerColor = playerColor;
        this.isCity = isCity;
    }

    public static Settlement parse(String str) {
        String[] split = str.split(" ");
        return new Settlement(PlayerColor.parse(split[0]), Boolean.parseBoolean(split[1]));
    }
}
