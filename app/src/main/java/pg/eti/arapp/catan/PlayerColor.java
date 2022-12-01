package pg.eti.arapp.catan;

public enum PlayerColor {
    BLUE,
    RED,
    ORANGE;

    public static PlayerColor parse(String str) {
        switch(str) {
            case "red": return RED;
            case "orange": return ORANGE;
            case "blue": return BLUE;
        }
        throw new RuntimeException("no such player color: " + str);
    }
}
