# Self-hosted infrastructure for the assignment

The composition prepares all three required infrastructure components:

- **OpenProject** (`http://host:8080`) - issue/work tracker, assignees, dates, boards and % complete.
- **Gitea** (`http://host:3000`, SSH `2222`) - Git repository.
- **Jenkins** (`http://host:8081`) - build/CI server.

## Launch

```bash
cd infra
docker compose up -d
```

Before use on department resources:
1. replace `OPENPROJECT_SECRET_KEY_BASE`;
2. put services behind HTTPS/reverse proxy if they are accessible outside the lab network;
3. create accounts for Поляков, Михалев and Ткачев;
4. import `docs/tracker_backlog.csv` into the tracker or create the listed work packages manually;
5. create a Gitea repository and push this project;
6. connect Jenkins to the repository and use the root `Jenkinsfile`.

The supplied compose file is a **ready-to-deploy configuration**, not a claim that services have already been deployed on department servers: that requires server access supplied by the instructor/department.
