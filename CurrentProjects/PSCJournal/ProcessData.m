clear;
name = "smalley"; % Change the name on this line

eval(sprintf('load Data/LightMediumHeavy/%s_heavy.mat;',name)); % Update the path for loading
eval(sprintf('load Data/LightMediumHeavy/%s_medium.mat;',name)); % Update the path for loading
eval(sprintf('load Data/LightMediumHeavy/%s_light.mat;',name)); % Update the path for loading

Nfft = 128;
samples = 40000;
eval(sprintf("a = pwelch(%s_heavy,boxcar(Nfft),0,Nfft,'twosided');",name));
eval(sprintf("b = pwelch(%s_medium,boxcar(Nfft),0,Nfft,'twosided');",name));
eval(sprintf("c = pwelch(%s_light,boxcar(Nfft),0,Nfft,'twosided');",name));
for runs = 1:samples
    afft(:,runs) = fftshift(a(:,runs));
    bfft(:,runs) = fftshift(b(:,runs));
    cfft(:,runs) = fftshift(c(:,runs));
end
eval(sprintf('%s_heavy_linear = afft;',name));
eval(sprintf('%s_medium_linear = bfft;',name));
eval(sprintf('%s_light_linear = cfft;',name));
adb = 10*log10(abs(afft));
bdb = 10*log10(abs(bfft));
cdb = 10*log10(abs(cfft));
eval(sprintf('%s_heavy_db = adb;',name));
eval(sprintf('%s_medium_db = bdb;',name));
eval(sprintf('%s_light_db = cdb;',name));

eval(sprintf('save Data/LightMediumHeavy/linear/%s_light_linear.mat %s_light_linear;',name,name));
eval(sprintf('save Data/LightMediumHeavy/dB/%s_light_db.mat %s_light_db;',name,name));
eval(sprintf('save Data/LightMediumHeavy/linear/%s_medium_linear.mat %s_medium_linear;',name,name));
eval(sprintf('save Data/LightMediumHeavy/dB/%s_medium_db.mat %s_medium_db;',name,name));
eval(sprintf('save Data/LightMediumHeavy/linear/%s_heavy_linear.mat %s_heavy_linear;',name,name));
eval(sprintf('save Data/LightMediumHeavy/dB/%s_heavy_db.mat %s_heavy_db;',name,name));