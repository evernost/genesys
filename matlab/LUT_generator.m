close all
clear all
clc

%% Settings
LUT_size  = 256;
N_bits    = 32;
LUT_file  = '../dev/genesys_core/Core/Src/wave_LUT.txt';
wave_mode = 'organ';

%% Signal generation

if strcmp(wave_mode, 'steps')
  a     = 0.8;
  steps = 8;

  d = 4*a/steps;
  u = [linspace(-a, a-d, steps/2)'; linspace(a, -a+d, steps/2)'];
  s = kron(u, ones(LUT_size/steps,1));

elseif strcmp(wave_mode, 'sine')
  %s  = 0.8*sin(2*pi*(0:(LUT_size-1))'/LUT_size) + (0.8/3)*sin(2*pi*3*(0:(LUT_size-1))'/LUT_size) + (0.8/5)*sin(2*pi*5*(0:(LUT_size-1))'/LUT_size);
  s  = 0.99*sin(2*pi*(0:(LUT_size-1))'/LUT_size);
elseif strcmp(wave_mode, 'saw')
  a     = 0.8;
  steps = 16;

  u = linspace(-a, a, steps)';
  s = kron(u, ones(LUT_size/steps,1));
elseif strcmp(wave_mode, 'organ')
  s  = 0.8*sin(2*pi*(0:(LUT_size-1))'/LUT_size) + (0.8/3)*sin(2*pi*3*(0:(LUT_size-1))'/LUT_size) + (0.8/5)*sin(2*pi*5*(0:(LUT_size-1))'/LUT_size);
else
  error('unrecognized wave mode.')
end
  
plot(s, '-o')
grid on

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
