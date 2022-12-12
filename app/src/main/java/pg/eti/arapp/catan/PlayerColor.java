package pg.eti.arapp.catan;

public enum PlayerColor {
    BLUE,
    RED,
    ORANGE;

    public static PlayerColor parse(String str) {
        str = str.trim();
        switch(str) {
            case "red": return RED;
            case "orange": return ORANGE;
            case "blue": return BLUE;
        }
        throw new RuntimeException("no such player color: " + str);
    }
}
