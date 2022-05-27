close all
clear all
clc

% Settings
LUT_size  = 256;
epsilon   = 0.001;
f_s       = 48000;
t_sim     = 2.0;
t_95      = 0.5;
LUT_file  = '../dev/genesys_core/Core/Src/exp_LUT.txt';

z = -(LUT_size-1)*log(epsilon)/LUT_size;
LUT_exp = exp(-z*(0:(LUT_size-1))'/LUT_size);

c = (LUT_size-1)*log(0.05)/(log(epsilon)*f_s);

N_pts = round(t_sim*f_s);
accu  = 0.0;
amp = zeros(N_pts, 1);
t = (0:(N_pts-1))'/f_s;

for k = 1:N_pts
  amp(k) = LUT_exp(floor(accu) + 1);
  accu = accu + (c/t_95);
  if (floor(accu) >= 256)
    break
  end
end

amp_th = exp(t*log(0.05)/t_95);

figure
subplot(1,2,1)
    plot(t,amp,t,amp_th)
    grid on
    
subplot(1,2,2)
    plot(t,amp-amp_th)
    grid on
    
disp(sprintf('Constant c            = %0.20f', c))
disp(sprintf('Accumulator increment = %0.15f', (c/t_95)))


%% LUT construction

fileID = fopen(LUT_file, 'w');
for k = 1:LUT_size
  % EOL 
  if (k == LUT_size)
    fprintf(fileID, '%0.15ff', s(k));
  else
    fprintf(fileID, '%0.15ff,\n', s(k));
  end
end
fclose(fileID);