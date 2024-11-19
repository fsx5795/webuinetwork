import shutil
import sys
if sys.platform.startswith('linux'):
    shutil.copy('./ui/index.html', './build/linux/x86_64/debug')
    shutil.copy('./ui/index.css', './build/linux/x86_64/debug')
    shutil.copy('./ui/index.js', './build/linux/x86_64/debug')
elif sys.platform.startswith('win32') or sys.platform.startswith('cygwin'):
    shutil.copy('./ui/index.html', './build/windows/x64/debug')
    shutil.copy('./ui/index.css', './build/windows/x64/debug')
    shutil.copy('./ui/index.js', './build/windows/x64/debug')