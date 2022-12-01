package pg.eti.arapp.catan.coord;

import java.util.Objects;

public class EdgeCoord {
    public CellCoord origin;
    public short side;

    public EdgeCoord(CellCoord origin, short side) {
        this.origin = origin;
        this.side = side;
    }

    public static EdgeCoord parse(String values) {
        String[] split = values.split("/");
        return new EdgeCoord(CellCoord.parse(split[0]), Short.parseShort(split[1]));
    }

    @Override
    public boolean equals(Object o) {
        if (this == o) return true;
        if (o == null || getClass() != o.getClass()) return false;
        EdgeCoord edgeCoord = (EdgeCoord) o;
        return side == edgeCoord.side && origin.equals(edgeCoord.origin);
    }

    @Override
    public int hashCode() {
        return Objects.hash(origin, side);
    }
}
