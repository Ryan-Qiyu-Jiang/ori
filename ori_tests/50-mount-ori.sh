cd $TEMP_DIR
mkdir -p $MTPOINT

$ORIFS_EXE -o repo=$SOURCE_REPO $MTPOINT
sleep 1.5

ls -lah $MTPOINT
$PYTHON $SCRIPTS/compare.py "$SOURCE_REPO" "$MTPOINT"

umount $MTPOINT

