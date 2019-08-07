% This file is loads in data and generates heatmaps for Harrison's,
% Smalley's, and Camacho's old offices.
% This was done using new data gathered in the empty offices, and used to
% ensure that Harrison's office still has an advantage over the other two

clear; close all;
load SPECIALFOLDER/processed.mat;

hmax = max(max(harrisonComplete));
smax = max(max(smalleyComplete));
cmax = max(max(camachoComplete));

figure();
imagesc(harrisonComplete);
ax1 = gca; title('Harrison (Max = ' + string(hmax) + ')');
colorbar;
caxis([0 500]);
ax1.YDir = 'normal'; drawnow;

figure();
imagesc(smalleyComplete);
ax2 = gca; title('Smalley (Max = ' + string(smax) + ')');
colorbar;
caxis([0 500]);
ax2.YDir = 'normal'; drawnow;

figure();
imagesc(camachoComplete);
ax3 = gca; title('Camacho (Max = ' + string(cmax) + ')');
colorbar;
caxis([0 500]);
ax3.YDir = 'normal'; drawnow;