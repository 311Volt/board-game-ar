package pg.eti.arapp.catan;

import java.util.HashMap;

import pg.eti.arapp.catan.coord.CellCoord;
import pg.eti.arapp.catan.coord.EdgeCoord;
import pg.eti.arapp.catan.coord.VertexCoord;

public class BoardInfo {
    public HashMap<CellCoord, String> cellTypes;
    public HashMap<EdgeCoord, PlayerColor> roads;
    public HashMap<VertexCoord, Settlement> settlements;

    public BoardInfo(HashMap<CellCoord, String> cellTypes, HashMap<EdgeCoord, PlayerColor> roads, HashMap<VertexCoord, Settlement> settlements) {
        this.cellTypes = cellTypes;
        this.roads = roads;
        this.settlements = settlements;
    }

    private enum ParseMode {
        NONE, CELLS, ROADS, SETTLEMENTS
    };

    public static BoardInfo parse(String data) {
        String[] lines = data.split("\n");

        HashMap<CellCoord, String> cellTypes = new HashMap<>();
        HashMap<EdgeCoord, PlayerColor> roads = new HashMap<>();
        HashMap<VertexCoord, Settlement> settlements = new HashMap<>();

        ParseMode mode = ParseMode.NONE;
        for(String line: lines) {
            switch (line) {
                case "cells": mode = ParseMode.CELLS; continue;
                case "roads": mode = ParseMode.ROADS; continue;
                case "settlements": mode = ParseMode.SETTLEMENTS; continue;
                default: break;
            }

            String[] split = line.split(":");
            if(mode == ParseMode.CELLS) {
                cellTypes.put(CellCoord.parse(split[0]), split[1]);
            } else if(mode == ParseMode.ROADS) {
                roads.put(EdgeCoord.parse(split[0]), PlayerColor.parse(split[1]));
            } else if(mode == ParseMode.SETTLEMENTS) {
                settlements.put(VertexCoord.parse(split[0]), Settlement.parse(split[1]));
            }

        }

        return new BoardInfo(cellTypes, roads, settlements);
    }
}
