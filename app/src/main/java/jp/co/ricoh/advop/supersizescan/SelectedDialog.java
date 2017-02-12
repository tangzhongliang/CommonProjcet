package jp.co.ricoh.advop.supersizescan;

import android.app.AlertDialog;
import android.app.Dialog;
import android.content.Context;
import android.content.DialogInterface;
import android.graphics.Bitmap;
import android.graphics.Canvas;
import android.graphics.drawable.Drawable;
import android.view.Gravity;
import android.view.KeyEvent;
import android.view.LayoutInflater;
import android.view.View;
import android.view.Window;
import android.widget.AdapterView;
import android.widget.ArrayAdapter;
import android.widget.LinearLayout;
import android.widget.ListView;
import android.widget.TextView;

import java.util.ArrayList;

import jp.co.ricoh.advop.mini.cheetahminiutil.ui.dialog.BaseDialog;
import jp.co.ricoh.advop.mini.cheetahminiutil.util.Buzzer;

/**
 * Copyright (C) 2013-2016 RICOH Co.,LTD
 * All rights reserved
 */

public class SelectedDialog {
    private final ArrayList<String> data;
    private Dialog dlg;
    private final Context context;
    AdapterView.OnItemClickListener listener;
    private ListView listView;
    private int buttonResId;
    private int title;

    protected SelectedDialog(Context context, ArrayList<String> arrayList) {
        this.context = context;
        this.data = arrayList;
        createDialog3(context);
        dlg.setOnKeyListener(new DialogInterface.OnKeyListener() {
            @Override
            public boolean onKey(DialogInterface dialog, int keyCode, KeyEvent event) {
                if (event.getAction() == KeyEvent.ACTION_DOWN && keyCode == KeyEvent.KEYCODE_BACK) {
                    Buzzer.play(Buzzer.BUZZER_NACK);
                    return true;
                }
                return false;
            }
        });
    }

    private void createDialog2(Context context) {
        dlg = new Dialog(context, R.style.base_dialog);
        LinearLayout layout = new LinearLayout(context);

        layout.setBackgroundColor(context.getResources().getColor(R.color.white));
        layout.setOrientation(LinearLayout.VERTICAL);
        View contentView = getContentView(context);
        TextView title = (TextView) LayoutInflater.from(context).inflate(getTitleResId(), null, false);
        TextView cancel = (TextView) LayoutInflater.from(context).inflate(getButtonResId(), null, false);
        TextView divider = new TextView(context);
        cancel.setOnClickListener(new View.OnClickListener() {
            @Override
            public void onClick(View v) {
                dlg.dismiss();
            }
        });
        title.setText(getTitle());
        layout.addView(title);
        layout.addView(contentView);
        cancel.setText(R.string.bt_cancel);
        cancel.setGravity(Gravity.CENTER);
        layout.addView(cancel);
        dlg.getWindow().setContentView(layout);
    }

    private void createDialog3(Context context) {
        dlg = new Dialog(context, R.style.base_dialog);
        View contentView = LayoutInflater.from(context).inflate(R.layout.select_dialog_layout, null, false);
        TextView title = (TextView) contentView.findViewById(R.id.title);
        TextView cancel = (TextView) contentView.findViewById(R.id.cancel);
        listView = (ListView) contentView.findViewById(R.id.list_view);
        listView.setAdapter(new ArrayAdapter<String>(context, R.layout.orient_list_item, getData()));
        cancel.setOnClickListener(new View.OnClickListener() {
            @Override
            public void onClick(View v) {
                dlg.dismiss();
            }
        });
        dlg.getWindow().setContentView(contentView);
    }

    private View getContentView(Context context) {
        listView = new ListView(context);
        listView.setAdapter(new ArrayAdapter<String>(context, R.layout.orient_list_item, getData()));
        return listView;
    }

    public ArrayList<String> getData() {
        return data;
    }

    public void setListener(AdapterView.OnItemClickListener listener) {
        listView.setOnItemClickListener(listener);
    }

    public int getStyle() {
        return R.style.select_dialog;
    }

    public boolean hasTitle() {
        return true;
    }

    public void show() {
        dlg.show();
    }

    public void dismiss() {
        dlg.dismiss();
    }

    public int getTitleResId() {
        return android.R.layout.simple_list_item_1;
    }

    public int getButtonResId() {
        return android.R.layout.simple_list_item_1;
    }

    public int getTitle() {
        return R.string.app_name;
    }
}
