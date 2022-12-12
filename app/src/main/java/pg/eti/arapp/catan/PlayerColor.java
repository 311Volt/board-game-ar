package pg.eti.arapp.catan;

import androidx.annotation.NonNull;

public enum PlayerColor {
    BLUE ("Blue"),
    RED ("Red"),
    ORANGE ("Orange");

    private final String name;
    private final String smallName;

    PlayerColor(String name) {
        this.name = name;
        smallName = name.toLowerCase();
    }

    public static PlayerColor parse(String str) {
        str = str.trim();
        switch(str) {
            case "red": return RED;
            case "orange": return ORANGE;
            case "blue": return BLUE;
        }
        throw new RuntimeException("no such player color: " + str);
    }

    public String getName() {
        return name;
    }

    public String getSmallName() {
        return smallName;
    }
}
