function check_sample_math_output(record_dir)
% Creates plot with 8 subplots showing error from each operation.
%
% Input: directory of the recording

% load original recordings
ch1_orig = load_open_ephys_data_faster(fullfile(record_dir, '102_CH1.continuous'));
ch2_orig = load_open_ephys_data_faster(fullfile(record_dir, '102_CH2.continuous'));

maxRecordedValue = 0.195 * double(intmax('int16'));
minRecordedValue = 0.195 * double(intmin('int16'));

clip = @(signal) max(minRecordedValue, min(maxRecordedValue, signal));

figure('Position', [200, 200, 1450, 675]);

% add constant
% consistent quantization error of ~-0.035 uV is normal, due to 100 not bein a multiple of the bitVolts.
subplot(241);
plot(clip(ch1_orig + 100) - load_open_ephys_data_faster(fullfile(record_dir, '103_CH1.continuous')), 'r');
hold on;
plot(clip(ch2_orig + 100) - load_open_ephys_data_faster(fullfile(record_dir, '103_CH2.continuous')), 'b');
title('Add constant');
legend('ch1', 'ch2');

% subtract constant
subplot(242);
plot(clip(ch1_orig - 100) - load_open_ephys_data_faster(fullfile(record_dir, '111_CH1.continuous')), 'r');
hold on;
plot(clip(ch2_orig - 100) - load_open_ephys_data_faster(fullfile(record_dir, '111_CH2.continuous')), 'b');
title('Subtract constant');
legend('ch1', 'ch2');

% multiply by constant
subplot(243);
plot(clip(ch1_orig .* 100) - load_open_ephys_data_faster(fullfile(record_dir, '112_CH1.continuous')), 'r');
hold on;
plot(clip(ch2_orig .* 100) - load_open_ephys_data_faster(fullfile(record_dir, '112_CH2.continuous')), 'b');
title('Multiply by constant');
legend('ch1', 'ch2');

% divide by constant
subplot(244);
plot(clip(ch1_orig ./ 100) - load_open_ephys_data_faster(fullfile(record_dir, '113_CH1.continuous')), 'r');
hold on;
plot(clip(ch2_orig ./ 100) - load_open_ephys_data_faster(fullfile(record_dir, '113_CH2.continuous')), 'b');
title('Divide by constant');
legend('ch1', 'ch2');

% add channel
subplot(245);
plot(clip(ch1_orig .* 2) - load_open_ephys_data_faster(fullfile(record_dir, '114_CH1.continuous')), 'r');
hold on;
plot(clip(ch1_orig + ch2_orig) - load_open_ephys_data_faster(fullfile(record_dir, '114_CH2.continuous')), 'b');
title('Add channel');
legend('ch1', 'ch2');

% subtract channel
subplot(246);
plot(-load_open_ephys_data_faster(fullfile(record_dir, '115_CH1.continuous')), 'r');
hold on;
plot(clip(ch2_orig - ch1_orig) - load_open_ephys_data_faster(fullfile(record_dir, '115_CH2.continuous')), 'b');
title('Subtract channel');
legend('ch1', 'ch2');

% multiply by channel
subplot(247);
plot(clip(ch1_orig .^ 2) - load_open_ephys_data_faster(fullfile(record_dir, '116_CH1.continuous')), 'r');
hold on;
plot(clip(ch1_orig .* ch2_orig) - load_open_ephys_data_faster(fullfile(record_dir, '116_CH2.continuous')), 'b');
title('Multiply by channel');
legend('ch1', 'ch2');

% divide by channel
subplot(248);
plot(1 - load_open_ephys_data_faster(fullfile(record_dir, '117_CH1.continuous')), 'r');
hold on;
plot(clip(ch2_orig ./ ch1_orig) - load_open_ephys_data_faster(fullfile(record_dir, '117_CH2.continuous')), 'b');
title('Divide by channel');
legend('ch1', 'ch2');

end
