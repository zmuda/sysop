set terminal svg
set output "parent_real.svg"
plot 'fork.tmp' using 2:20 title 'real_fork' linecolor rgb 'blue' with lines,'vfork.tmp' using 2:20 title 'real_vfork' linecolor rgb 'red' with lines,'clone.tmp' using 2:20 title 'real_clone' linecolor rgb 'green' with lines,'vclone.tmp' using 2:20 title 'real_vclone' linecolor rgb 'cyan' with lines

set output "parent_system.svg"
plot 'fork.tmp' using 2:22 title 'system_fork' linecolor rgb 'blue' with lines,'vfork.tmp' using 2:22 title 'system_vfork' linecolor rgb 'red' with lines,'clone.tmp' using 2:22 title 'system_clone' linecolor rgb 'green' with lines,'vclone.tmp' using 2:22 title 'system_vclone' linecolor rgb 'cyan' with lines

set output "parent_user.svg"
plot 'fork.tmp' using 2:24 title 'user_fork' linecolor rgb 'blue' with lines,'vfork.tmp' using 2:24 title 'user_vfork' linecolor rgb 'red' with lines,'clone.tmp' using 2:24 title 'user_clone' linecolor rgb 'green' with lines,'vclone.tmp' using 2:24 title 'user_vclone' linecolor rgb 'cyan' with lines

set output "parent_user+system.svg"
plot 'fork.tmp' using 2:26 title 'user+system_fork' linecolor rgb 'blue' with lines,'vfork.tmp' using 2:26 title 'user+system_vfork' linecolor rgb 'red' with lines,'clone.tmp' using 2:26 title 'user+system_clone' linecolor rgb 'green' with lines,'vclone.tmp' using 2:26 title 'user+system_vclone' linecolor rgb 'cyan' with lines

set output "children_real.svg"
plot 'fork.tmp' using 2:12 title 'real_fork' linecolor rgb 'blue' with lines,'vfork.tmp' using 2:12 title 'real_vfork' linecolor rgb 'red' with lines,'clone.tmp' using 2:12 title 'real_clone' linecolor rgb 'green' with lines,'vclone.tmp' using 2:12 title 'real_vclone' linecolor rgb 'cyan' with lines

set output "children_system.svg"
plot 'fork.tmp' using 2:14 title 'system_fork' linecolor rgb 'blue' with lines,'vfork.tmp' using 2:14 title 'system_vfork' linecolor rgb 'red' with lines,'clone.tmp' using 2:14 title 'system_clone' linecolor rgb 'green' with lines,'vclone.tmp' using 2:14 title 'system_vclone' linecolor rgb 'cyan' with lines

set output "children_user.svg"
plot 'fork.tmp' using 2:16 title 'user_fork' linecolor rgb 'blue' with lines,'vfork.tmp' using 2:16 title 'user_vfork' linecolor rgb 'red' with lines,'clone.tmp' using 2:16 title 'user_clone' linecolor rgb 'green' with lines,'vclone.tmp' using 2:16 title 'user_vclone' linecolor rgb 'cyan' with lines

set output "children_user+system.svg"
plot 'fork.tmp' using 2:18 title 'user+system_fork' linecolor rgb 'blue' with lines,'vfork.tmp' using 2:18 title 'user+system_vfork' linecolor rgb 'red' with lines,'clone.tmp' using 2:18 title 'user+system_clone' linecolor rgb 'green' with lines,'vclone.tmp' using 2:18 title 'user+system_vclone' linecolor rgb 'cyan' with lines


set output "parent+children_real.svg"
plot 'fork.tmp' using 2:4 title 'real_fork' linecolor rgb 'blue' with lines,'vfork.tmp' using 2:4 title 'real_vfork' linecolor rgb 'red' with lines,'clone.tmp' using 2:4 title 'real_clone' linecolor rgb 'green' with lines,'vclone.tmp' using 2:4 title 'real_vclone' linecolor rgb 'cyan' with lines

set output "parent+children_system.svg"
plot 'fork.tmp' using 2:6 title 'system_fork' linecolor rgb 'blue' with lines,'vfork.tmp' using 2:6 title 'system_vfork' linecolor rgb 'red' with lines,'clone.tmp' using 2:6 title 'system_clone' linecolor rgb 'green' with lines,'vclone.tmp' using 2:6 title 'system_vclone' linecolor rgb 'cyan' with lines

set output "parent+children_user.svg"
plot 'fork.tmp' using 2:8 title 'user_fork' linecolor rgb 'blue' with lines,'vfork.tmp' using 2:8 title 'user_vfork' linecolor rgb 'red' with lines,'clone.tmp' using 2:8 title 'user_clone' linecolor rgb 'green' with lines,'vclone.tmp' using 2:8 title 'user_vclone' linecolor rgb 'cyan' with lines

set output "parent+children_user+system.svg"
plot 'fork.tmp' using 2:10 title 'user+system_fork' linecolor rgb 'blue' with lines,'vfork.tmp' using 2:10 title 'user+system_vfork' linecolor rgb 'red' with lines,'clone.tmp' using 2:10 title 'user+system_clone' linecolor rgb 'green' with lines,'vclone.tmp' using 2:10 title 'user+system_vclone' linecolor rgb 'cyan' with lines
