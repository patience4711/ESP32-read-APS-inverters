


void pwm_init(void)
{
    ledc_timer_config_t ledc_timer = {
        .speed_mode       = LEDC_MODE,
        .duty_resolution  = LEDC_DUTY_RES,
        .timer_num        = LEDC_TIMER,       
        .freq_hz          = LEDC_FREQUENCY,
        .clk_cfg          = LEDC_AUTO_CLK
    };
    ledc_timer_config(&ledc_timer);

    ledc_channel_config_t ledc_channel = {
        .gpio_num       = LEDC_OUTPUT_IO,      
        .speed_mode     = LEDC_MODE,
        .channel        = LEDC_CHANNEL,
        .intr_type      = LEDC_INTR_DISABLE,      
        .timer_sel      = LEDC_TIMER,
        .duty           = 0,
        .hpoint         = 0
    };
    ledc_channel_config(&ledc_channel);
    ledc_fade_func_install(0);
}

    // ledc_timer_config_t ledc_timer = {
    //     .speed_mode       = LEDC_MODE,      //LEDC_LOW_SPEED_MODE
    //     .duty_resolution  = LEDC_DUTY_RES,  // LEDC_TIMER_13_BIT
    //     .timer_num        = LEDC_TIMER,     //LEDC_TIMER_0
    //     .freq_hz          = LEDC_FREQUENCY,  // Set output frequency at 4 kHz
    //     .clk_cfg          = LEDC_AUTO_CLK
    // };

void set_pwm(int targetlevel) {
  //consoleOut("set_pwm value = " + String(value));
  int value = (targetlevel * LEDC_MAX_DUTY) / 100;
  ledc_set_duty(LEDC_MODE, LEDC_CHANNEL, value);
  // Update duty to apply the new value
  ledc_update_duty(LEDC_MODE, LEDC_CHANNEL);
}

void fade_pwm(int targetlevel) {
  // the value is in range 0 - 100 so we have to calculate
  int value = (targetlevel * LEDC_MAX_DUTY) / 100;
  // LEDC_MODE, LEDC_CHANNEL, target duty, time = set in ms
  //consoleOut("fade_pwm value = " + String(value)); 
  ledc_set_fade_with_time(LEDC_MODE, LEDC_CHANNEL, value, 1000); 
  ledc_fade_start(LEDC_MODE, LEDC_CHANNEL, LEDC_FADE_NO_WAIT);
}