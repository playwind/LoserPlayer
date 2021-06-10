package com.playwind.example

import android.Manifest
import android.content.pm.PackageManager
import android.os.Bundle
import android.view.WindowManager
import android.widget.SeekBar
import androidx.appcompat.app.AppCompatActivity
import androidx.core.content.ContextCompat
import com.playwind.example.databinding.ActivityFullscreenBinding
import com.playwind.loserplayer.LoserPlayer

class FullscreenActivity : AppCompatActivity() {
    private lateinit var binding: ActivityFullscreenBinding
    private var isFullscreen: Boolean = false
    private val player = LoserPlayer()

    override fun onCreate(savedInstanceState: Bundle?) {
        super.onCreate(savedInstanceState)
        window.addFlags(WindowManager.LayoutParams.FLAG_KEEP_SCREEN_ON)  //应用运行时，保持屏幕高亮，不锁屏

        binding = ActivityFullscreenBinding.inflate(layoutInflater)
        setContentView(binding.root)
        supportActionBar?.setDisplayHomeAsUpEnabled(true)

        initView()
        checkPermissions()
    }

    private fun checkPermissions() {
        if (ContextCompat.checkSelfPermission(
                this,
                Manifest.permission.WRITE_EXTERNAL_STORAGE
            ) != PackageManager.PERMISSION_GRANTED
        ) {
            requestPermissions(arrayOf(Manifest.permission.WRITE_EXTERNAL_STORAGE), 1000)
        }
    }

    override fun onRequestPermissionsResult(
        requestCode: Int,
        permissions: Array<out String>,
        grantResults: IntArray
    ) {
        super.onRequestPermissionsResult(requestCode, permissions, grantResults)
    }

    private fun initView() {
        binding.btnPause.setOnClickListener {
            if (player.isPaused()) {
                player.resume()
            } else {

                player.pause()
            }
        }

        binding.btnPlay.setOnClickListener {
            //player.open("/sdcard/1080.mp4")
            //player.open("/sdcard/trailer.mp4")
            //player.open("rtmp://live.hkstv.hk.lxdns.com/live/hks")
            player.open("http://ivi.bupt.edu.cn/hls/cctv1hd.m3u8")
            updateProgress()
        }

        binding.btnStop.setOnClickListener {
            player.close()
        }

        binding.seekBar.setOnSeekBarChangeListener(object : SeekBar.OnSeekBarChangeListener {
            override fun onProgressChanged(seekBar: SeekBar, progress: Int, fromUser: Boolean) {
                if (fromUser) {
                    player.seek(progress / seekBar.max.toDouble())
                }
            }

            override fun onStartTrackingTouch(seekBar: SeekBar?) {
            }

            override fun onStopTrackingTouch(seekBar: SeekBar?) {
            }

        })
    }

    private fun updateProgress() {
        Thread {
            while (!isDestroyed) {
                binding.seekBar.progress = (player.progress() * 100).toInt()
                try {
                    Thread.sleep(200)
                } catch (e: InterruptedException) {
                    e.printStackTrace()
                }
            }
        }.start()
    }

    override fun onSupportNavigateUp(): Boolean {
        onBackPressed()
        return true
    }

    override fun onDestroy() {
        super.onDestroy()
        player.close()
    }

}