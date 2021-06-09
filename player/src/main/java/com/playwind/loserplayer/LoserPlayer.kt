package com.playwind.loserplayer

import android.view.Surface

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

    external fun initView(surface: Surface)

    external fun open(url: String)

    external fun close()

    external fun seek(progress: Double)

    external fun isPaused(): Boolean

    external fun progress(): Double

    external fun pause()

    external fun resume()
}