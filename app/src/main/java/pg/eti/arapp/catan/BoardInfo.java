package pg.eti.arapp.catan;

import android.util.Pair;

import java.util.ArrayDeque;
import java.util.HashMap;
import java.util.Objects;
import java.util.Queue;

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


    /* Performs a modified breadth-first search algorithm. Finds all possible settlement
     * coordinates reachable through roads by the player that owns the settlement at "begin".
     */
    private HashMap<VertexCoord, Integer> roadBreadthFirstSearch(VertexCoord begin) {

        HashMap<VertexCoord, Integer> result = new HashMap<>();
        Queue<Pair<VertexCoord, Integer>> bfsQueue = new ArrayDeque<>();
        bfsQueue.add(new Pair<>(begin, 0));

        if(!settlements.containsKey(begin)) {
            throw new RuntimeException("Cannot begin BFS from a non-occupied vertex");
        }
        PlayerColor targetColor = Objects.requireNonNull(settlements.get(begin)).playerColor;

        while(!bfsQueue.isEmpty()) {
            var entry = bfsQueue.remove();
            int bfsLevel = entry.second;
            VertexCoord coord = entry.first;

            result.put(coord, bfsLevel);

            if(!settlements.containsKey(coord)) {
                continue;
            }

            for(VertexCoord neigh: coord.possibleNeighbors()) {
                PlayerColor road = roads.get(EdgeCoord.ofVertexPair(coord, neigh));
                if(road == targetColor) {
                    bfsQueue.add(new Pair<>(neigh, bfsLevel+1));
                }
            }
        }
        return result;
    }

}
