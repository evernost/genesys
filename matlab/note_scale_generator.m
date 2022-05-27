close all
clear all
clc

%% Settings
LUT_file  = '../dev/genesys_core/Core/Src/note_scale.txt';
scale_mode = 'equal';

%% Signal generation
f = zeros(128,1);
if strcmp(scale_mode, 'equal')
    f = 440.0*2.^(((0:127)'-69)/12);
end

%% LUT construction

fileID = fopen(LUT_file, 'w');
for k = 1:128
  % EOL 
  if (k == 128)
    fprintf(fileID, '%0.15ff', f(k));
  else
    fprintf(fileID, '%0.15ff,\n', f(k));
  end
end
fclose(fileID);
