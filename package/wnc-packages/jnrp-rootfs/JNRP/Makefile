PFRMVER			:= 1.0
#SQL_FILES = $(wildcard  system/config/*.sql)
SQL_FILES = $(wildcard  system/config_$(PROJECT)/*.sql)

all: db release

db:
	echo "BEGIN;" >> system.sql
	for i in $(SQL_FILES); \
		do \
			if test $$i != system/config_$(PROJECT)/user.sql; then \
			if [ -e $$i ]; then \
				cat $$i >> system.sql; \
			fi ; \
			fi ; \
		done

	echo "COMMIT;" >> system.sql

release:
	echo "$(PFRMVER)" > version
	sed  -i "s/.*jnrpVer.*/config.system\[1\]\[\"jnrpVer\"\] = \"$(PFRMVER)\"/" jnr-cfg.ascii.default_$(PROJECT)
