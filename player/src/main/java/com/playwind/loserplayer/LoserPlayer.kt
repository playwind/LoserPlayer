package com.playwind.loserplayer

/**
 * @author forrest
 * @date 2021/5/27
 * @desc
 * @email forrest_zheng@intretech.com
 */
class LoserPlayer {

    companion object {
        init {
            System.loadLibrary("loser-player")
        }
    }

    external fun getPlayerInfo(): String
}