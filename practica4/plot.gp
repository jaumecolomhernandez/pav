set terminal pngcairo
set output 'out.png' 
set multiplot
set xrange [-270.221:776.171]
set yrange [-3.270222:0.66704]
set parametric
set trange [0:2*pi]
plot 160.757 + 1 * 91.1911 * cos(t), -2.53302 + 1 * 0.239257 * sin(t) title "" 
plot 1.6466 + 1 * 0.215747 * cos(t), -1.27267 + 1 * 0.154913 * sin(t) title "" 
plot 197.473 + 1 * 169.699 * cos(t), -2.06538 + 1 * 0.351424 * sin(t) title "" 
plot 272.514 + 1 * 131.294 * cos(t), -2.76295 + 1 * 0.253636 * sin(t) title "" 
plot 252.975 + 1 * 261.598 * cos(t), -0.751146 + 1 * 0.709093 * sin(t) title "" 
unset parametric
