package org.kde.ringkde;

import org.qtproject.qt5.android.bindings.QtActivity;
import org.qtproject.qt5.android.bindings.QtApplication;
import android.util.Log;
import android.content.Context;
import android.os.Bundle;
import android.media.AudioAttributes;
import java.util.Locale;
import java.lang.String;

public class Ringkde extends QtActivity
{
    public Ringkde()
    {
    }

    /** Called when the activity is first created. */
    @Override
    public void onCreate(Bundle savedInstanceState) {
            super.onCreate(savedInstanceState);
    }

    public void onInit(int status)
    {
    }

    @Override
    protected void onDestroy() {
            super.onDestroy();
            /*if (m_tts != null) {
                m_tts.shutdown();
            }*/
    }

}

