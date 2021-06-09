package com.playwind.example

import android.content.Context
import android.util.AttributeSet
import android.view.SurfaceHolder
import android.view.SurfaceView
import android.view.View
import com.playwind.loserplayer.LoserPlayer

/**
 * @author forrest
 * @date 2021/6/4
 * @desc
 * @email forrest_zheng@intretech.com
 */
class PlayerSurfaceView : SurfaceView, View.OnClickListener, SurfaceHolder.Callback {
    private lateinit var player: LoserPlayer

    constructor(context: Context?) : super(context) {
        init()
    }

    constructor(context: Context?, attrs: AttributeSet?) : super(context, attrs) {
        init()
    }

    private fun init() {
        setOnClickListener(this)
        holder.addCallback(this)
        player = LoserPlayer()
    }

    override fun onClick(v: View?) {

    }

    override fun surfaceChanged(holder: SurfaceHolder, format: Int, width: Int, height: Int) {

    }

    override fun surfaceDestroyed(holder: SurfaceHolder) {

    }

    override fun surfaceCreated(holder: SurfaceHolder) {
        player.initView(holder.surface)
    }
}