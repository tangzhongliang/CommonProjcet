package jp.co.ricoh.advop.supersizescan;

import jp.co.ricoh.advop.mini.cheetahminiutil.application.BaseApplication;
import jp.co.ricoh.advop.mini.cheetahminiutil.logic.InitParameters;
import jp.co.ricoh.advop.mini.cheetahminiutil.ssdk.function.common.SmartSDKApplication;

public class CheetahApplication extends BaseApplication{

	@Override
	protected InitParameters getInitParameters() {
		InitParameters initParameters = new InitParameters();
		initParameters.setAppLogTag("supersizescan");
		initParameters.setMainActivityClass(MainActivity.class);
//		initParameters.setPrintAvailable(true);
		initParameters.setScanAvailable(true);
		return initParameters;
	}
}
