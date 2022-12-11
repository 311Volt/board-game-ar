package pg.eti.arapp.catan.coord;

import java.util.Objects;

public class EdgeCoord {
    public CellCoord origin;
    public short side;

    public EdgeCoord(CellCoord origin, int side) {
        this.origin = origin;
        this.side = (short)side;
    }

    public static EdgeCoord parse(String values) {
        values = values.trim();
        String[] split = values.split("/");
        return new EdgeCoord(CellCoord.parse(split[0].trim()), Short.parseShort(split[1].trim()));
    }

    public static EdgeCoord ofVertexPair(VertexCoord a, VertexCoord b) {
        int originDist = a.origin.distance(b.origin);
        if(a.high == b.high || originDist > 1 || (originDist == 1 && a.origin.y == b.origin.y)) {
            throw new RuntimeException("vertex coord pair does not form an edge");
        }

        CellCoord diff = a.origin.diff(b.origin);
        if(a.origin.equals(b.origin)) {
            return new EdgeCoord(a.origin, 0);
        }
        if(diff.compareTo(CellCoord.ORIGIN) > 0) {
            return new EdgeCoord(b.origin, diff.y);
        } else {
            return new EdgeCoord(a.origin, -diff.y);
        }
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
